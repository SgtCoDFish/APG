/*
 * APGTest.cpp
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
 */

#include <cstdlib>

#include <iostream>
#include <memory>
#include <chrono>
#include <vector>
#include <numeric>
#include <utility>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <Tmx.h>
#include <Game.hpp>
#include <SXXDL.hpp>
#include <APGTest.hpp>
#include <SDLTmxRenderer.hpp>
#include <SDLTileset.hpp>

const std::string ASSET_PREFIX = "assets/";

bool APGTest::init() {
	if (hasError()) {
		std::cerr << "Failed SDLGame initialisation:\n" << getErrorMessage() << std::endl;
		return false;
	}

	renderer = SXXDL::make_renderer_ptr(
			SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED));

	if (renderer == nullptr) {
		std::cerr << "Couldn't create renderer:\n" << SDL_GetError() << std::endl;
		return false;
	}

	SDL_SetRenderDrawColor(renderer.get(), 0xFF, 0x00, 0x00, 0xFF);

	map = std::make_unique<Tmx::Map>();
	map->ParseFile(ASSET_PREFIX + "world1.tmx");

	if (map->HasError()) {
		std::cerr << "Error loading map: " << map->GetErrorText() << std::endl;
		return false;
	}

	tmxRenderer = std::make_unique<APG::SDLTmxRenderer>(map, renderer);

	if (tmxRenderer->hasError()) {
		std::cerr << "Error creating tmxRenderer: " << tmxRenderer->getErrorMessage() << std::endl;
		return false;
	}

	return true;
}

void APGTest::render(float deltaTime) {
	SDL_RenderClear(renderer.get());

	tmxRenderer->renderAll();

	SDL_RenderPresent(renderer.get());
}

int main(int argc, char *argv[]) {
	APG::SDLGame::sdlWindowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
	auto rpg = std::make_unique<APGTest>();

	if (!rpg->init()) {
		return EXIT_FAILURE;
	}

	auto map = rpg->getMap();

	std::cout << "Map width: " << map->GetWidth() << ", height: " << map->GetHeight() << ".\n";
	std::cout << "Has " << map->GetNumLayers() << " layers.\n";

	bool done = false;

	auto startTime = std::chrono::high_resolution_clock::now();
	std::vector<float> timesTaken;
	while (!done) {
		auto timeNow = std::chrono::high_resolution_clock::now();
		float deltaTime =
				std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - startTime).count()
						/ 1000.0f;

		startTime = timeNow;
		timesTaken.push_back(deltaTime);

		done = rpg->update(deltaTime);

		if (timesTaken.size() >= 1000) {
			const float sum = std::accumulate(timesTaken.begin(), timesTaken.end(), 0.0f);

			const float fps = 1 / (sum / timesTaken.size());

			std::cout << "FPS: " << fps << std::endl;

			timesTaken.clear();
		}
	}

	return EXIT_SUCCESS;
}
