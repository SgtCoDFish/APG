/*
 * Copyright (c) 2014, 2015 See AUTHORS file.
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

#include <glm/vec2.hpp>

#include "tmxparser/TmxMap.h"
#include "tmxparser/TmxLayer.h"
#include "tmxparser/TmxTileset.h"
#include "tmxparser/TmxImage.h"
#include "tmxparser/TmxTile.h"
#include "tmxparser/TmxTileLayer.h"
#include "tmxparser/TmxObjectGroup.h"
#include "tmxparser/TmxObject.h"

#include "APG/SXXDL.hpp"
#include "APG/core/APGeasylogging.hpp"
#include "APG/core/APGCommon.hpp"
#include "APG/graphics/Tileset.hpp"
#include "APG/graphics/AnimatedSprite.hpp"
#include "APG/tiled/TmxRenderer.hpp"
#include "APG/internal/Assert.hpp"

std::unordered_map<std::string, std::shared_ptr<APG::Tileset>> APG::TmxRenderer::tmxTilesets;

APG::TmxRenderer::TmxRenderer(Tmx::Map *map) :
		        map { map } {
	loadTilesets();
	loadObjects();
}

void APG::TmxRenderer::loadTilesets() {
	initialiseStaticTilesets();

	const auto logger = el::Loggers::getLogger("APG");

	const auto tileWidth = map->GetTileWidth();
	const auto tileHeight = map->GetTileHeight();

	logger->info("Loading map %v with (tileWidth, tileHeight) = (%vpx, %vpx)", map->GetFilename(), tileWidth,
	        tileHeight);

#ifdef APG_IGNORE_ANIMATED_TILES
	logger->info("Note: APG_IGNORE_ANIMATED_TILES is set.");
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

		const auto tilesetName = map->GetFilepath() + tileset->GetImage()->GetSource();

		const auto tilesetExists = tmxTilesets.find(tilesetName);
		Tileset * loadedTileset = nullptr;

		if (tilesetExists != tmxTilesets.end()) {
			logger->info("Using previously loaded tileset for \"%v\"", tilesetName);

			loadedTileset = tilesetExists->second.get();
		} else {
			logger->info("Loading tileset \"%v\" (first GID = %v, has %v special tiles, spacing = %vpx)", tilesetName,
			        tileset->GetFirstGid(), tileset->GetTiles().size(), tileset->GetSpacing());

			tmxTilesets.emplace(tilesetName, std::make_shared<Tileset>(tilesetName, tileset));

			tilesets.emplace_back(std::shared_ptr<Tileset>(tmxTilesets.at(tilesetName)));
			loadedTileset = tilesets.back().get();
		}

		REQUIRE(loadedTileset != nullptr, "Couldn't load/find tileset when loading map");

		const auto spacing = loadedTileset->getSpacing();

		/*
		 * Note that tileset->GetTiles() only returns tiles which have something special about them, e.g. a property or an animation.
		 *
		 * This means we need to create our Sprites by iterating through the image sequentially.
		 *
		 * The GIDs start at 1 for the first tileset, and increase by 1 going right.
		 * When the right edge of the image is reached, we go down and back to the left of the image.
		 */
		uint32_t tileID = 0, x = 0, y = 0;
		while (true) {
			const auto tileGID = calculateTileGID(tileset, tileID);

			loadedSprites.emplace_back(loadedTileset, x, y, tileWidth, tileHeight);

			auto &sprite = loadedSprites.back();
			sprite.setHash(tileGID);

			sprites.insert(std::pair<uint64_t, SpriteBase *>(sprite.getHash(), &sprite));

			x += tileWidth + spacing;
			++tileID;
			if (x >= loadedTileset->getWidth()) {
				x = 0;
				y += tileHeight + spacing;

				if (y >= loadedTileset->getHeight()) {
					break;
				}
			}
		}

		for (const auto &tile : tileset->GetTiles()) {
			const auto tileGID = calculateTileGID(tileset, tile);

			logger->verbose(1, "Special tile #%v has %v properties.", tileGID, tile->GetProperties().GetSize());

			if (el::Loggers::verboseLevel() >= 1) {
				for (const auto &property : tile->GetProperties().GetList()) {
					logger->verbose(1, "Property \"%v\" = \"%v\"", property.first, property.second);
				}
			}

#ifndef APG_IGNORE_ANIMATED_TILES
			if (tile->IsAnimated()) {
				logger->verbose(1, "Animated tile has %v tiles, with total duration %vms.", tile->GetFrameCount(),
				        tile->GetTotalDuration());

				const auto &frames = tile->GetFrames();
				std::vector<SpriteBase *> framePointers;
				framePointers.reserve(frames.size());

				// TODO: This ignores per-frame durations in the file, should be improved.
				// Length is stored in ms, convert to seconds
				float length = (float) tile->GetTotalDuration() / 1000.0f;
				unsigned int framesLoaded = 0;

				for (const auto &frame : frames) {
					const auto gid = calculateTileGID(tileset, frame.GetTileID());
					logger->verbose(2, "Frame #%v: ID %v, GID %v, duration = %vms.", framesLoaded, frame.GetTileID(),
					        gid, frame.GetDuration());

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

void APG::TmxRenderer::loadObjects() {
	const auto logger = el::Loggers::getLogger("APG");

	for (auto &group : map->GetObjectGroups()) {
		logger->info("Loading object group \"%v\", has %v objects.", group->GetName(), group->GetNumObjects());
		std::vector<TiledObject> objects;

		for (auto &obj : group->GetObjects()) {
			const auto gid = obj->GetGid();

			if (gid == 0) {
				logger->verbose(5, "Ignoring non-tile object \"%v\"", obj->GetName());
				continue;
			}

			objects.emplace_back(obj->GetX(), obj->GetY() - map->GetTileHeight(), sprites[gid]);
		}

		objectGroups.emplace(group->GetName(), objects);
	}
}

void APG::TmxRenderer::renderAll(float deltaTime) {
	for (auto &animation : loadedAnimatedSprites) {
		animation.update(deltaTime);
	}

	for (const auto &layer : map->GetLayers()) {
		if (layer->IsVisible()) {
			switch (layer->GetLayerType()) {
			case Tmx::LayerType::TMX_LAYERTYPE_TILE: {
				renderLayer((Tmx::TileLayer *) layer);
				break;
			}

			case Tmx::LayerType::TMX_LAYERTYPE_OBJECTGROUP: {
				renderObjectGroup(objectGroups[layer->GetName()]);
				break;
			}

			default: {
				el::Loggers::getLogger("APG")->warn("Unsupported layer type for layer \"%v\"", layer->GetName());
				break;
			}
			}
		}
	}
}

uint64_t APG::TmxRenderer::calculateTileGID(Tmx::Tileset * tileset, Tmx::Tile * tile) {
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

void APG::TmxRenderer::initialiseStaticTilesets() {
	if (tmxTilesets.empty()) {
		tmxTilesets.reserve(internal::MAX_SUPPORTED_TEXTURES);
	}
}
