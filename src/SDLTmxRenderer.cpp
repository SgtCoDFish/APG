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

#include <string>
#include <sstream>

#include "tmxparser/Tmx.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "APG/SXXDL.hpp"
#include "APG/TmxRenderer.hpp"
#include "APG/SDLTmxRenderer.hpp"

#include "APG/internal/Log.hpp"

APG::SDLTmxRenderer::SDLTmxRenderer(Tmx::Map * const map, SXXDL::renderer_ptr &renderer) :
		TmxRenderer(map), renderer { renderer } {

	if (map->GetOrientation() != Tmx::MapOrientation::TMX_MO_ORTHOGONAL) {
		APG_LOG("SDLTmxRenderer only supports orthogonal maps.");
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
	const auto tile_width = map->GetTileWidth();
	const auto tile_height = map->GetTileHeight();

	auto src_rect = SDL_Rect { 0, 0, tile_width, tile_height };
	auto dst_rect = SDL_Rect { 0, 0, tile_width, tile_height };


	if (layer->GetVisible()) {
		for (int y = 0; y < layer->GetHeight(); y++) {
			for (int x = 0; x < layer->GetWidth(); x++) {

				const unsigned int tile_id = layer->GetTileId(x, y);

				if (tile_id == 0) {
					continue;
				}

				const auto tileset_index = layer->GetTileTilesetIndex(x, y);
				const auto &current_tileset = tilesets[tileset_index];
				const auto &sdl_tileset = sdlTextures[tileset_index];

				const int tileset_x = tile_id % current_tileset->getWidthInTiles();
				const int tileset_y = tile_id / current_tileset->getWidthInTiles();

				src_rect.x = tileset_x * tile_width;
				src_rect.y = tileset_y * tile_height;

				dst_rect.x = (int) position.x + x * tile_width;
				dst_rect.y = (int) position.y + y * tile_height;

				if (SDL_RenderCopy(renderer.get(), sdl_tileset.get(), &src_rect, &dst_rect) < 0) {
					APG_LOG(SDL_GetError());
					return;
				}
			}
		}
	}
}
