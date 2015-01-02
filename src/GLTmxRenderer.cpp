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

	const auto tileWidth = (float) map->GetTileWidth();
	const auto tileHeight = (float) map->GetTileHeight();

	for (int y = 0; y < layer->GetHeight(); y++) {
		for (int x = 0; x < layer->GetWidth(); x++) {
			const auto &tile = layer->GetTile(x, y);

			if (tile.id == 0) {
				continue;
			}

			Tileset *tileset = tilesets[tile.tilesetId].get();
			const float tileX = position.x + x * tileWidth;
			const float tileY = position.y + y * tileHeight;

			const float texX = tile.id % tileset->getWidthInTiles();
			const float texY = tile.id / tileset->getWidthInTiles();

			batch.draw(tileset, tileX, tileY, tileWidth, tileHeight, texX, texY, tileWidth,
					tileHeight);
		}
	}
}
