/*
 * GLTmxRenderer.cpp
 * Copyright (C) 2014, 2015 Ashley Davis (SgtCoDFish)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "Tmx.h"

#include <glm/vec2.hpp>

#include "GLTmxRenderer.hpp"
#include "Sprite.hpp"

APG::GLTmxRenderer::GLTmxRenderer(Tmx::Map * const map, SpriteBatch &inBatch) :
		TmxRenderer(map), batch(inBatch) {
}

void APG::GLTmxRenderer::renderAll() {
	batch.begin();

	TmxRenderer::renderAll();

	batch.end();
}

void APG::GLTmxRenderer::renderLayer(Tmx::Layer * const layer) {
	if (!layer->IsVisible()) {
		return;
	}

	const auto tileWidth = map->GetTileWidth();
	const auto tileHeight = map->GetTileHeight();

	for (int y = 0; y < layer->GetHeight(); y++) {
		for (int x = 0; x < layer->GetWidth(); x++) {
			const auto &tile = layer->GetTile(x, y);

			if (tile.id == 0) {
				continue;
			}

			Tileset *tileset = getTilesetByID(tile.tilesetId);
			const int32_t tileX = position.x + x * tileWidth;
			const int32_t tileY = position.y + y * tileHeight;

			const int32_t texX = tile.id % tileset->getWidthInTiles();
			const int32_t texY = tile.id / tileset->getWidthInTiles();

			Sprite sprite { tileset, texX * tileWidth, texY * tileHeight, tileWidth, tileHeight };

			batch.draw(&sprite, tileX, tileY);
		}
	}
}
