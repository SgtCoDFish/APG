/*
 * Copyright (c) 2014, 2015 See AUTHORS file.
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

#include <cstdlib>

#include <memory>
#include <chrono>
#include <vector>
#include <numeric>
#include <utility>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <tmxparser/Tmx.h>

#include "APG/SXXDL.hpp"
#include "APG/core/APGeasylogging.hpp"
INITIALIZE_EASYLOGGINGPP
#include "APG/core/Game.hpp"
#include "APG/tiled/SDLTmxRenderer.hpp"

#include "test/APGSDLRenderTest.hpp"

const std::string ASSET_PREFIX = "assets/";

bool APGSDLRenderTest::init() {
	const auto logger = el::Loggers::getLogger("APG");
	renderer = SXXDL::make_renderer_ptr(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED));

	if (renderer == nullptr) {
		logger->fatal("Couldn't create SDL_Renderer: %v", SDL_GetError());
		return false;
	}

	SDL_SetRenderDrawColor(renderer.get(), 0x50, 0xAC, 0x3D, 0xFF);

	auto mapOne = std::make_unique<Tmx::Map>();
	mapOne->ParseFile(ASSET_PREFIX + "world1.tmx");

	if (mapOne->HasError()) {
		logger->fatal("Error loading tmx map: %v", mapOne->GetErrorText());
		return false;
	}

	auto mapTwo = std::make_unique<Tmx::Map>();
	mapTwo->ParseFile(ASSET_PREFIX + "sample_indoor.tmx");

	if (mapTwo->HasError()) {
		logger->fatal("Error loading tmx map: %v", mapTwo->GetErrorText());
		return false;
	}

	rendererOne = std::make_unique<APG::SDLTmxRenderer>(std::move(mapOne), renderer);
	rendererTwo = std::make_unique<APG::SDLTmxRenderer>(std::move(mapTwo), renderer);

	currentRenderer = rendererOne.get();

	return true;
}

void APGSDLRenderTest::render(float deltaTime) {
	SDL_RenderClear(renderer.get());

	if (inputManager->isKeyJustPressed(SDL_SCANCODE_SPACE)) {
		if (currentRenderer == rendererOne.get()) {
			currentRenderer = rendererTwo.get();
		} else {
			currentRenderer = rendererOne.get();
		}
	}

	currentRenderer->renderAll(deltaTime);

	SDL_RenderPresent(renderer.get());
}

int main(int argc, char *argv[]) {
	START_EASYLOGGINGPP(argc, argv);

	const std::string windowTitle("APG GLTmxRenderer Example");
	const uint32_t windowWidth = 1280;
	const uint32_t windowHeight = 720;

	const auto logger = el::Loggers::getLogger("APG");

	auto rpg = std::make_unique<APGSDLRenderTest>(windowTitle, windowWidth, windowHeight);

	if (!rpg->init()) {
		return EXIT_FAILURE;
	}

	bool done = false;

	auto startTime = std::chrono::high_resolution_clock::now();
	std::vector<float> timesTaken;

	while (!done) {
		auto timeNow = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - startTime).count() / 1000.0f;

		startTime = timeNow;
		timesTaken.push_back(deltaTime);

		done = rpg->update(deltaTime);

		if (timesTaken.size() >= 1000) {
			const float sum = std::accumulate(timesTaken.begin(), timesTaken.end(), 0.0f);

			const float fps = 1 / (sum / timesTaken.size());

			logger->info("FPS: ", fps);

			timesTaken.clear();
		}
	}

	return EXIT_SUCCESS;
}

