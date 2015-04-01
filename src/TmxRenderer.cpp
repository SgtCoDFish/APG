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

#include "tmxparser/TmxMap.h"
#include "tmxparser/TmxLayer.h"
#include "tmxparser/TmxTileset.h"
#include "tmxparser/TmxImage.h"
#include "tmxparser/TmxTile.h"

#include "APG/APGCommon.hpp"
#include "APG/ErrorBase.hpp"
#include "APG/Tileset.hpp"
#include "APG/SXXDL.hpp"
#include "APG/TmxRenderer.hpp"
#include "APG/AnimatedSprite.hpp"

#include <glm/vec2.hpp>

APG::TmxRenderer::TmxRenderer(Tmx::Map *map) :
		map { map } {
	loadTilesets();
}

void APG::TmxRenderer::loadTilesets() {
	const uint32_t tileWidth = map->GetTileWidth();
	const uint32_t tileHeight = map->GetTileHeight();

	for (const auto &tileset : map->GetTilesets()) {
		const auto tilesetName = map->GetFilepath()
				+ tileset->GetImage()->GetSource();

		auto loadedTileset = tileset_ptr(new Tileset(tilesetName, map));

		// now we've loaded the tileset, we need to iterate once to build a Sprite
		// for each tile, and then again to build up the animated sprites which
		// could rely on tiles which are loaded after the animated tile is.

		for (const auto &tile : tileset->GetTiles()) {
            const uint32_t texX = tile->GetId()
                    % loadedTileset->getWidthInTiles();
            const uint32_t texY = tile->GetId()
                    / loadedTileset->getWidthInTiles();

            const uint64_t spriteHash = calculateTileHash(
                    loadedTileset.get(), tile);

            Sprite newSprite = Sprite(loadedTileset.get(), texX * tileWidth,
                    texY * tileHeight, tileWidth, tileHeight);
            loadedSprites.emplace_back(std::move(newSprite));

            sprites.insert(
                    std::pair<uint64_t, SpriteBase *>(spriteHash,
                            &(loadedSprites.back())));
		}

		std::cout << "Loaded " << tileset->GetName() << std::endl;

		// now iterate again for animated sprites since all the frames must be loaded

		for (const auto &tile : tileset->GetTiles()) {
			if (tile->IsAnimated()) {
				std::vector<Sprite *> frameVec(tile->GetFrameCount());

				for (const auto frame : tile->GetFrames()) {
					const auto frameHash = calculateTileHash(
							loadedTileset.get(), frame.GetTileID());

					try {
						frameVec.emplace_back((Sprite *) sprites.at(frameHash));
					} catch (std::out_of_range &oor) {
						setErrorState(
								"Invalid frame id when loading animated sprite.");
						return;
					}
				}

				const uint64_t tileHash = calculateTileHash(loadedTileset.get(),
						tile->GetId());

				loadedAnimatedSprites.emplace_back(0.15f, frameVec,
						AnimationMode::LOOP);

				sprites.insert(
						std::pair<uint64_t, SpriteBase *>(tileHash,
								&(loadedAnimatedSprites.back())));
			}
		}
	}
}

void APG::TmxRenderer::renderAll() {
	for (const auto &layer : map->GetLayers()) {
		renderLayer(layer);
	}
}

uint64_t APG::TmxRenderer::calculateTileHash(const Tileset *tileset,
		Tmx::Tile * const tile) const {
	return calculateTileHash(tileset, tile->GetId());
}

uint64_t APG::TmxRenderer::calculateTileHash(const Tileset *tileset,
		int tileID) const {
	return MAX_SPRITES_PER_UNIT + (tileset->getGLTextureUnit() + 1) + tileID;
}

