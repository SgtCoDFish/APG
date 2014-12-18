/*
 * ArrPeeGee.cpp
 * Copyright (C) 2014 Ashley Davis (SgtCoDFish)
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
 *
 */

#include <cstdlib>

#include <iostream>
#include <memory>
#include <chrono>
#include <vector>
#include <numeric>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <Tmx.h>
#include <ArrPeeGee.hpp>

using surface_ptr = std::unique_ptr<SDL_Surface, void(*)(SDL_Surface *)>;
using texture_ptr = std::unique_ptr<SDL_Texture, void(*)(SDL_Texture *)>;
const std::string ASSET_PREFIX = "assets/";
std::vector<texture_ptr> tileset_surfaces;
std::vector<int> tileset_widths;
int solidLayer = 0;

RPG::RPG::RPG() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		std::cerr << "Couldn't initialise SDL2:\n" << SDL_GetError() << std::endl;
		initFailure = true;
		return;
	}

	const int imageFlags = IMG_INIT_PNG;
	if (!IMG_Init(imageFlags) & imageFlags) {
		std::cerr << "Couldn't initialise SDL2_image:\n" << IMG_GetError() << std::endl;
		initFailure = true;
		return;
	}
}

RPG::RPG::~RPG() {
	IMG_Quit();
	SDL_Quit();
}

bool RPG::RPG::init() {
	if (initFailure) {
		std::cerr << "Call to init() after failed SDL2 initialisation.\n";
		return false;
	}

	window = std::unique_ptr<SDL_Window, void (*)(SDL_Window *)>(
			SDL_CreateWindow("ArrPeeGee", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640,
					480, SDL_WINDOW_SHOWN), SDL_DestroyWindow);

	if (window == nullptr) {
		std::cerr << "Couldn't create window:\n" << SDL_GetError() << std::endl;
		return false;
	}

	renderer = std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer *)>(
			SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED), SDL_DestroyRenderer);

	if (renderer == nullptr) {
		std::cerr << "Couldn't create renderer:\n" << SDL_GetError() << std::endl;
		return false;
	}

	SDL_SetRenderDrawColor(renderer.get(), 0xFF, 0x00, 0x00, 0xFF);

	map = std::make_unique<Tmx::Map>();
	map->ParseFile(ASSET_PREFIX + "world1.tmx");

	if (map->HasError()) {
		std::cout << "Error loading map: " << map->GetErrorText() << std::endl;
		return false;
	}

	auto tilesets = map->GetTilesets();

	for (auto &tileset : tilesets) {
		auto bad_surface = surface_ptr(
				IMG_Load((ASSET_PREFIX + tileset->GetImage()->GetSource()).c_str()),
				SDL_FreeSurface);

		if (bad_surface == nullptr) {
			std::cerr << "Couldn't load " << tileset->GetImage()->GetSource() << ":\n"
					<< IMG_GetError() << std::endl;
			return false;
		}

		auto texture = texture_ptr(SDL_CreateTextureFromSurface(renderer.get(), bad_surface.get()),
				SDL_DestroyTexture);

		if (texture == nullptr) {
			std::cerr << "Couldn't optimise image: " << tileset->GetImage()->GetSource() << ":\n"
					<< IMG_GetError() << std::endl;
			return false;
		}

		int w;
		SDL_QueryTexture(texture.get(), nullptr, nullptr, &w, nullptr);
		tileset_surfaces.push_back(std::move(texture));
		tileset_widths.push_back(w);
	}

	std::cout << "--- Loaded " << tileset_surfaces.size() << " tilesets ---\n";

	return true;
}

bool RPG::RPG::update(float deltaTime) {
	while (SDL_PollEvent(&eventStore)) {
		if (eventStore.type == SDL_QUIT) {
			return true;
		}
	}

	render(deltaTime);

	return false;
}

void RPG::RPG::render(float deltaTime) {
	SDL_RenderClear(renderer.get());

	int renderCount = 0;
	for (auto &layer : map->GetLayers()) {
		if (layer->GetVisible()) {
			for (int y = 0; y < layer->GetHeight(); y++) {
				for (int x = 0; x < layer->GetWidth(); x++) {
					const auto tileset_index = layer->GetTileTilesetIndex(x, y);
					const auto &current_tileset = tileset_surfaces[tileset_index];

					const unsigned int tile_id = layer->GetTileId(x, y);
					if (tile_id == 0) {
						continue;
					}
					const int width_in_tiles = tileset_widths[tileset_index] / 32;
					const int tileset_y = tile_id / width_in_tiles;
					const int tileset_x = tile_id % width_in_tiles;

//				std::cout << "\n(" << renderCount << ")Tile ID: " << tile_id << "\nWidth In Tiles: "
//						<< width_in_tiles << "\ntileset_x: " << tileset_x << "\ntileset_y: "
//						<< tileset_y << std::endl;

					const auto src_rect = SDL_Rect { tileset_x * 32, tileset_y * 32, 32, 32 };
					const auto dst_rect = SDL_Rect { x * 32, y * 32, 32, 32 };

					SDL_RenderCopy(renderer.get(), current_tileset.get(), &src_rect, &dst_rect);
					renderCount++;
				}
			}
		}
	}

	SDL_RenderPresent(renderer.get());
}

int main(int argc, char *argv[]) {
	auto rpg = std::make_unique<RPG::RPG>();

	if (!rpg->init()) {
		return EXIT_FAILURE;
	}

	auto map = rpg->getMap();

	std::cout << "Map width: " << map->GetWidth() << ", height: " << map->GetHeight() << ".\n";
	std::cout << "Has " << map->GetNumLayers() << " layers.\n";

	int layerCount = 0;
	for (auto &layer : map->GetLayers()) {
		auto name = layer->GetName();

		if (name == "solidobs") {
			solidLayer = layerCount;
			std::cout << "Solidobs layer = " << solidLayer << std::endl;
			auto tile = layer->GetTile(0, 0);

			std::cout << "On solidobs:\n\tTileset name: "
					<< map->GetTileset(tile.tilesetId)->GetName() << "\n\tTileID: " << tile.id
					<< "\n";

			std::cout << "\t(0,0) == (1,0): " << (layer->GetTile(1, 0).id == tile.id) << std::endl;
			std::cout << "\t(0,0) == (1,1): " << (layer->GetTile(1, 1).id == tile.id) << std::endl;
		}

		layerCount++;
	}

	bool done = false;

	auto startTime = std::chrono::high_resolution_clock::now();
	std::vector<float> timesTaken(100);
	while (!done) {
		auto timeNow = std::chrono::high_resolution_clock::now();
		float deltaTime =
				std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - startTime).count()
						/ 1000.0f;

		startTime = timeNow;
		timesTaken.push_back(deltaTime);

		done = rpg->update(deltaTime);

		if(timesTaken.size() >= 100) {
			const float sum = std::accumulate(timesTaken.begin(), timesTaken.end(), 0.0f);
			const float fps = 1 / (sum / timesTaken.size());

			std::cout << "FPS: " << fps << std::endl;
			timesTaken.clear();
		}
	}

	return EXIT_SUCCESS;
}
