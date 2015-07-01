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
	const uint32_t tileWidth = map->GetTileWidth();
	const uint32_t tileHeight = map->GetTileHeight();

	for (const auto &tileset : map->GetTilesets()) {
		const auto tilesetName = map->GetFilepath() + tileset->GetImage()->GetSource();

		tilesets.emplace_back(tileset_ptr(new Tileset(tilesetName, map)));
		auto &loadedTileset = tilesets.back();

		if (loadedTileset == nullptr) {
			std::stringstream ss;
			ss << "Couldn't load: " << tilesetName;
			APG_LOG(ss.str().c_str());
			return;
		}

		// now we've loaded the tileset, we need to iterate once to build a Sprite
		// for each tile, and then again to build up the animated sprites which
		// could rely on tiles which are loaded after the animated tile is.

		// used to store the first frames of animations temporarily before they're moved into animations.
		std::unordered_map<uint64_t, Sprite> tempSprites;

		for (const auto &tile : tileset->GetTiles()) {
			const uint32_t texX = tile->GetId() % loadedTileset->getWidthInTiles();
			const uint32_t texY = tile->GetId() / loadedTileset->getWidthInTiles();

			const uint64_t spriteHash = calculateTileHash(loadedTileset.get(), tile);

//			std::cout << "(" << texX << ", " << texY << ") hash: " << spriteHash << std::endl;

			Sprite newSprite = Sprite(loadedTileset.get(), texX * tileWidth, texY * tileHeight, tileWidth, tileHeight);
			newSprite.setHash(spriteHash);

			if (!tile->IsAnimated()) {
				loadedSprites.emplace_back(std::move(newSprite));
			} else {
				tempSprites.insert(std::pair<uint64_t, Sprite>(spriteHash, std::move(newSprite)));
			}
		}

		for (auto &sprite : loadedSprites) {
			sprites.insert(std::pair<uint64_t, SpriteBase *>(sprite.getHash(), &sprite));
		}

		// now iterate again for animated sprites since all the frames must be loaded
		for (const auto &tile : tileset->GetTiles()) {
			if (tile->IsAnimated()) {
				std::cout << "Loading animated tile (" << tile->GetId() << ") with " << tile->GetFrameCount()
				        << " frames.\n";
				const uint64_t firstTileHash = calculateTileHash(loadedTileset.get(), tile->GetId());

				// TODO: Something is going wrong here, hence  the "dirty" hack in AnimatedSprite
				auto animSprite = AnimatedSprite(0.3f, std::move(tempSprites.at(firstTileHash)), AnimationMode::LOOP);
				tempSprites.erase(firstTileHash);
				animSprite.setHash(firstTileHash);

				for (const auto frame : tile->GetFrames()) {
					const auto frameHash = calculateTileHash(loadedTileset.get(), frame.GetTileID());

					if (frameHash == firstTileHash) {
						// the owned frame could appear multiple times so we add nullptr for each occurrence, then set it in the AnimatedSprite constructor
						animSprite.addFrame(nullptr);
					} else {
//						try {
							// will fail always since sprites isn't init yet
							// need to change animsprite constructor
							animSprite.addFrame((Sprite *) sprites[frameHash]);
//						} catch (std::out_of_range &oor) {
//							std::stringstream ss;
//							ss << "Invalid frame id when loading animated sprite: " << frameHash;
//							setErrorState(ss.str());
//							return;
//						}
					}
				}

				loadedAnimatedSprites.emplace_back(std::move(animSprite));
			}
		}
	}

	for (auto &animSprite : loadedAnimatedSprites) {
		sprites.insert(std::pair<uint64_t, SpriteBase *>(animSprite.getHash(), &animSprite));
	}

	std::cout << "Loaded " << tilesets.size() << " tilesets.\n";
}

void APG::TmxRenderer::renderAll(float deltaTime) {
	for (auto &animation : loadedAnimatedSprites) {
		animation.update(deltaTime);
	}

	for (const auto &layer : map->GetLayers()) {
		renderLayer(layer);
	}
}

uint64_t APG::TmxRenderer::calculateTileHash(const Tileset *tileset, Tmx::Tile * const tile) const {
	return calculateTileHash(tileset, tile->GetId());
}

uint64_t APG::TmxRenderer::calculateTileHash(const Tileset *tileset, int tileID) const {
	return MAX_SPRITES_PER_UNIT * (tileset->getGLTextureUnit() + 1) + tileID;
}

