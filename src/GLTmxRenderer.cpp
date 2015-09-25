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

#include <iostream>

#include <glm/vec2.hpp>

#include "tmxparser/Tmx.h"

#include "APG/GLTmxRenderer.hpp"
#include "APG/Sprite.hpp"

APG::GLTmxRenderer::GLTmxRenderer(Tmx::Map * const map, SpriteBatch * const inBatch) :
		        TmxRenderer(map),
		        batch(inBatch) {
}

void APG::GLTmxRenderer::renderAll(float deltaTime) {
	batch->begin();

	TmxRenderer::renderAll(deltaTime);

	batch->end();
}

void APG::GLTmxRenderer::renderLayer(Tmx::TileLayer * const layer) {
//	std::cout << "Rendering layer \"" << layer->GetName() << "\"\n";
	const uint32_t tileWidth = map->GetTileWidth();
	const uint32_t tileHeight = map->GetTileHeight();

	for (int y = 0; y < layer->GetHeight(); y++) {
		for (int x = 0; x < layer->GetWidth(); x++) {
			const uint32_t tileX = position.x + x * tileWidth;
			const uint32_t tileY = position.y + y * tileHeight;

			const auto &tile = layer->GetTile(x, y);

			if (tile.tilesetId == -1) {
				continue;
			}

			const auto tileHash = tile.gid;

			const auto ourSprite = sprites[tileHash];
			batch->draw(ourSprite, tileX, tileY);
		}
	}
}

void APG::GLTmxRenderer::renderObjectGroup(const std::vector<TiledObject> &objects) {
	for (const auto &obj : objects) {
		batch->draw(obj.sprite, obj.position.x, obj.position.y);
	}
}
