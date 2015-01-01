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
#include <iostream>

#include "Tmx.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "SXXDL.hpp"
#include "SDLTmxRenderer.hpp"
#include "SDLTileset.hpp"

APG::SDLTmxRenderer::SDLTmxRenderer(map_ptr &map, SXXDL::renderer_ptr &renderer) :
		map { map }, renderer { renderer } {
	if (map->GetOrientation() != Tmx::MapOrientation::TMX_MO_ORTHOGONAL) {
		setErrorState("SDLTmxRenderer only supports orthogonal maps.");
		return;
	}

	// load tilesets for use when rendering.
	for (const auto &tileset : map->GetTilesets()) {
		auto loaded_tileset = loadTileset(tileset);

		if (hasError()) {
			return;
		}

		tilesets.push_back(std::move(loaded_tileset));
	}

}

APG::tileset_ptr APG::SDLTmxRenderer::loadTileset(const Tmx::Tileset *tileset) {
	if (hasError()) {
		return nullptr;
	}

	auto bad_surface = SXXDL::make_surface_ptr(
			IMG_Load((map->GetFilepath() + tileset->GetImage()->GetSource()).c_str()));

	if (bad_surface == nullptr) {
		std::stringstream ss;
		ss << "Couldn't load " << tileset->GetImage()->GetSource() << ":\n" << IMG_GetError();

		setErrorState(ss.str());
		return nullptr;
	}

	auto texture = SXXDL::make_texture_ptr(
			SDL_CreateTextureFromSurface(renderer.get(), bad_surface.get()));

	if (texture == nullptr) {
		std::stringstream ss;
		ss << "Couldn't optimise image: " << tileset->GetImage()->GetSource() << ":\n"
				<< IMG_GetError() << std::endl;

		setErrorState(ss.str());
		return nullptr;
	}

	int w = -1, h = -1;
	if (SDL_QueryTexture(texture.get(), nullptr, nullptr, &w, &h) == -1) {
		std::stringstream ss;
		ss << "Invalid texture type: " << SDL_GetError() << "\n";
		setErrorState(ss.str());
		return nullptr;
	}

	if (w <= 0 || h <= 0) {
		std::stringstream ss;
		ss << "Invalid texture width/height: w = " << w << ", h = " << h << ".\nSDL: "
				<< SDL_GetError() << "\n";
		setErrorState(ss.str());
		return nullptr;
	}

	auto retVal = std::make_unique<SDLTileset>(map, w, h, texture);

	return retVal;
}

void APG::SDLTmxRenderer::renderAll() {
	if (hasError()) {
		return;
	}

	const auto tile_width = map->GetTileWidth();
	const auto tile_height = map->GetTileHeight();

	auto src_rect = SDL_Rect { 0, 0, tile_width, tile_height };
	auto dst_rect = SDL_Rect { 0, 0, tile_width, tile_height };


	int renderCount = 0;
	for (auto &layer : map->GetLayers()) {
		if (layer->GetVisible()) {
			for (int y = 0; y < layer->GetHeight(); y++) {
				for (int x = 0; x < layer->GetWidth(); x++) {
					const auto &current_tileset = tilesets[layer->GetTileTilesetIndex(x, y)];

					const unsigned int tile_id = layer->GetTileId(x, y);

					if (tile_id == 0) {
						continue;
					}

					const int tileset_x = tile_id % current_tileset->width_in_tiles;
					const int tileset_y = tile_id / current_tileset->width_in_tiles;

					src_rect.x = tileset_x * tile_width;
					src_rect.y = tileset_y * tile_height;

					dst_rect.x = (int)position.x + x * tile_width;
					dst_rect.y = (int)position.y + y * tile_height;

					if(SDL_RenderCopy(renderer.get(), current_tileset->texture.get(), &src_rect,
							&dst_rect) < 0) {
						setErrorState(SDL_GetError());
						return;
					}
					renderCount++;
				}
			}
		}
	}
}
