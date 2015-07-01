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
#include <iostream>
#include <string>

#include "tmxparser/TmxMap.h"
#include "tmxparser/TmxLayer.h"
#include "tmxparser/TmxTileset.h"
#include "tmxparser/TmxImage.h"
#include "tmxparser/TmxTile.h"

#include "APG/APGCommon.hpp"
#include "APG/Tileset.hpp"
#include "APG/SXXDL.hpp"
#include "APG/TmxRenderer.hpp"
#include "APG/AnimatedSprite.hpp"

#include "APG/internal/Assert.hpp"
#include "APG/internal/Log.hpp"

#include <glm/vec2.hpp>

APG::TmxRenderer::TmxRenderer(Tmx::Map *map) :
		map { map } {
	loadTilesets();
}

void APG::TmxRenderer::loadTilesets() {
	const auto tileWidth = map->GetTileWidth();
	const auto tileHeight = map->GetTileHeight();

	/*
	 * We need to reserve space for our sprites or the vectors will be
	 * dynamically reallocated and we'll be left with a load of broken pointers.
	 */
	reserveSpriteSpace();

	for (const auto &tileset : map->GetTilesets()) {
		REQUIRE(tileset->GetTileWidth() == tileWidth && tileset->GetTileHeight() == tileHeight,
		        "Only tilesets with tile size consistent with the map are currently supported.");
		REQUIRE(tileset->GetSpacing() == 0, "Only tilesets with 0 spacing between tiles are currently supported.");

		const auto tilesetName = map->GetFilepath() + tileset->GetImage()->GetSource();

		std::cout << "Loading tileset " << tilesetName << " with first GID = " << tileset->GetFirstGid() << " and "
		        << tileset->GetTiles().size() << " special tiles.\n";

		tilesets.emplace_back(tileset_ptr(new Tileset(tilesetName, map)));
		auto &loadedTileset = tilesets.back();

		if (loadedTileset == nullptr) {
			std::stringstream ss;
			ss << "Couldn't load: " << tilesetName;
			APG_LOG(ss.str().c_str());
			return;
		}

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

			std::cout << "Loading special data for tile #" << tileGID << std::endl;

			for (const auto &property : tile->GetProperties().GetList()) {
				std::cout << "\tHas property \"" << property.first << "\", value = \"" << property.second << "\".\n";
			}

			if (tile->IsAnimated()) {
				std::cout << "\tAnimated tile, has " << tile->GetFrameCount() << " tiles, duration = "
				        << tile->GetTotalDuration() << "ms.\n";

				const auto &frames = tile->GetFrames();
				std::vector<SpriteBase *> framePointers;
				framePointers.reserve(frames.size());

				for (const auto &frame : frames) {
					const auto gid = calculateTileGID(tileset, frame.GetTileID());

					REQUIRE(sprites.find(gid) != sprites.end(), "Animation frame not loaded into sprites array.");
					std::cout << "\t > Frame has id = " << frame.GetTileID() << ", gid = " << gid << ".\n";

					framePointers.emplace_back(sprites[gid]);
				}

				// TODO: This ignores per-frame durations in the file, should be improved.
				loadedAnimatedSprites.emplace_back(0.3f, framePointers, AnimationMode::LOOP);
				sprites[tileGID] = &(loadedAnimatedSprites.back());
			}
		}
	}

	std::cout << "Loaded " << tilesets.size() << " tilesets.\n";
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
