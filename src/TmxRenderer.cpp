/*
 * Copyright (c) 2014, 2015 Ashley Davis (SgtCoDFish)
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdint>

#include <vector>
#include <unordered_map>
#include <utility>
#include <string>

#include "tmxparser/TmxMap.h"
#include "tmxparser/TmxLayer.h"
#include "tmxparser/TmxTileset.h"
#include "tmxparser/TmxImage.h"
#include "tmxparser/TmxTile.h"

#include "easylogging++.h"

#include "APG/APGCommon.hpp"
#include "APG/Tileset.hpp"
#include "APG/SXXDL.hpp"
#include "APG/TmxRenderer.hpp"
#include "APG/AnimatedSprite.hpp"

#include "APG/internal/Assert.hpp"

#include <glm/vec2.hpp>

APG::TmxRenderer::TmxRenderer(Tmx::Map *map) :
		map { map } {
	loadTilesets();
}

void APG::TmxRenderer::loadTilesets() {
	const auto logger = el::Loggers::getLogger("default");

	const auto tileWidth = map->GetTileWidth();
	const auto tileHeight = map->GetTileHeight();

	logger->info("Loading map %v with (tileWidth, tileHeight) = (%vpx, %vpx)", map->GetFilename(), tileWidth,
	        tileHeight);

#ifdef APG_IGNORE_ANIMATED_TILES
	logger->info("Note: APG_IGNORE_ANIMATED_TILES is set, so animated tiles might be ignored.");
#endif

	/*
	 * We need to reserve space for our sprites or the vectors will be
	 * dynamically reallocated during loading,
	 * and we'll be left with a load of broken pointers.
	 */
	reserveSpriteSpace();

	for (const auto &tileset : map->GetTilesets()) {
		REQUIRE(tileset->GetTileWidth() == tileWidth && tileset->GetTileHeight() == tileHeight,
		        "Only tilesets with tile size consistent with the map are currently supported.");
		REQUIRE(tileset->GetSpacing() == 0, "Only tilesets with 0 spacing between tiles are currently supported.");

		const auto tilesetName = map->GetFilepath() + tileset->GetImage()->GetSource();

		logger->info("Loading tileset %v (first GID = %v, has %v special tiles)", tilesetName, tileset->GetFirstGid(),
		        tileset->GetTiles().size());

		tilesets.emplace_back(tileset_ptr(new Tileset(tilesetName, map)));
		auto &loadedTileset = tilesets.back();

		/*
		 * Note that tileset->GetTiles() only returns tiles which have something special about them, e.g. a property or an animation.
		 *
		 * This means we need to create our Sprites by iterating through the image sequentially.
		 *
		 * The GIDs start at 1 for the first tileset, and increase by 1 going right.
		 * When the right edge of the image is reached, we go down and back to the left of the image.
		 */

		for (uint32_t y = 0; y < loadedTileset->getHeight(); y += tileHeight) {
			for (uint32_t x = 0; x < loadedTileset->getWidth(); x += tileWidth) {
				const auto tileID = (x / tileWidth) + (y / tileHeight) * loadedTileset->getWidthInTiles();
				const auto tileGID = calculateTileGID(tileset, tileID);

				loadedSprites.emplace_back(loadedTileset.get(), x, y, tileWidth, tileHeight);

				auto &sprite = loadedSprites.back();
				sprite.setHash(tileGID);

				sprites.insert(std::pair<uint64_t, SpriteBase *>(sprite.getHash(), &sprite));
			}
		}

		for (const auto &tile : tileset->GetTiles()) {
			const auto tileGID = calculateTileGID(tileset, tile);

			logger->info("Special tile #%v has %v properties.", tileGID, tile->GetProperties().GetSize());

			if (el::Loggers::verboseLevel() >= 1) {
				for (const auto &property : tile->GetProperties().GetList()) {
					logger->verbose(1, "Property \"%v\" = \"%v\"", property.first, property.second);
				}
			}

#ifndef APG_IGNORE_ANIMATED_TILES
			if (tile->IsAnimated()) {
				logger->verbose(1, "Animated tile has %v tiles, with total duration %vms.", tile->GetFrameCount(), tile->GetTotalDuration());

				const auto &frames = tile->GetFrames();
				std::vector<SpriteBase *> framePointers;
				framePointers.reserve(frames.size());

				// TODO: This ignores per-frame durations in the file, should be improved.
				// Length is stored in ms, convert to seconds
				float length = (float) tile->GetTotalDuration() / 1000.0f;
				unsigned int framesLoaded = 0;

				for (const auto &frame : frames) {
					const auto gid = calculateTileGID(tileset, frame.GetTileID());
					logger->verbose(2, "Frame #%v: ID %v, GID %v, duration = %vms.", framesLoaded, frame.GetTileID(), gid, frame.GetDuration());

					REQUIRE(sprites.find(gid) != sprites.end(), "Animation frame not loaded into sprites array.");

					framePointers.emplace_back(sprites[gid]);
					++framesLoaded;
				}

				loadedAnimatedSprites.emplace_back(length, framePointers, AnimationMode::LOOP);
				sprites[tileGID] = &(loadedAnimatedSprites.back());
			}
#endif
		}
	}
}

void APG::TmxRenderer::renderAll(float deltaTime) {
	for (auto &animation : loadedAnimatedSprites) {
		animation.update(deltaTime);
	}

	for (const auto &layer : map->GetTileLayers()) {
		renderLayer(layer);
	}
}

uint64_t APG::TmxRenderer::calculateTileGID(Tmx::Tileset *tileset, Tmx::Tile *tile) {
	return calculateTileGID(tileset, tile->GetId());
}

uint64_t APG::TmxRenderer::calculateTileGID(Tmx::Tileset *tileset, int tileID) {
	return tileset->GetFirstGid() + tileID;
}

void APG::TmxRenderer::reserveSpriteSpace() {
	int32_t tileCount = 0;
	int32_t animTileCount = 0;

	const auto &tilesets = map->GetTilesets();

	// Go through the tilesets counting all the static tiles and then individually counting animated tiles.
	for (const auto &tileset : tilesets) {
		tileCount += (tileset->GetImage()->GetWidth() / tileset->GetTileWidth())
		        * (tileset->GetImage()->GetHeight() / tileset->GetTileHeight());

		// Find animated tiles; there are probably far fewer animated than static, so reserving the same amount of space would be wasteful.
		for (const auto &tile : tileset->GetTiles()) {
			if (tile->IsAnimated()) {
				++animTileCount;
			}
		}
	}

	sprites.reserve(tileCount);
	loadedSprites.reserve(tileCount);
	loadedAnimatedSprites.reserve(animTileCount);
}
