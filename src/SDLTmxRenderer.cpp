/*
 * SDLTmxRenderer.cpp
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

#include <string>
#include <sstream>

#include "Tmx.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "SXXDL.hpp"
#include "TmxRenderer.hpp"
#include "SDLTmxRenderer.hpp"

APG::SDLTmxRenderer::SDLTmxRenderer(map_ptr &map, SXXDL::renderer_ptr &renderer) :
		TmxRenderer(map), renderer { renderer } {
	if (hasError()) {
		return;
	}

	if (map->GetOrientation() != Tmx::MapOrientation::TMX_MO_ORTHOGONAL) {
		setErrorState("SDLTmxRenderer only supports orthogonal maps.");
		return;
	}

	for (const auto &tileset : tilesets) {
		sdlTextures.emplace_back(
				SXXDL::make_sdl_texture_ptr(
						SDL_CreateTextureFromSurface(renderer.get(),
								tileset->getPreservedSurface())));
	}
}

void APG::SDLTmxRenderer::renderLayer(Tmx::Layer *layer) {
	if (hasError()) {
		return;
	}

	const auto tile_width = map->GetTileWidth();
	const auto tile_height = map->GetTileHeight();

	auto src_rect = SDL_Rect { 0, 0, tile_width, tile_height };
	auto dst_rect = SDL_Rect { 0, 0, tile_width, tile_height };


	if (layer->GetVisible()) {
		for (int y = 0; y < layer->GetHeight(); y++) {
			for (int x = 0; x < layer->GetWidth(); x++) {
				const auto tileset_index = layer->GetTileTilesetIndex(x, y);
				const auto &current_tileset = tilesets[tileset_index];
				const auto &sdl_tileset = sdlTextures[tileset_index];

				const unsigned int tile_id = layer->GetTileId(x, y);

				if (tile_id == 0) {
					continue;
				}

				const int tileset_x = tile_id % current_tileset->getWidthInTiles();
				const int tileset_y = tile_id / current_tileset->getWidthInTiles();

				src_rect.x = tileset_x * tile_width;
				src_rect.y = tileset_y * tile_height;

				dst_rect.x = (int) position.x + x * tile_width;
				dst_rect.y = (int) position.y + y * tile_height;

				if (SDL_RenderCopy(renderer.get(), sdl_tileset.get(), &src_rect, &dst_rect) < 0) {
					setErrorState(SDL_GetError());
					return;
				}
			}
		}
	}
}
