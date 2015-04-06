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

#include <cstdint>
#include <cstdlib>

#include <iostream>
#include <memory>
#include <chrono>
#include <vector>
#include <numeric>

#include <GL/glew.h>
#include <GL/gl.h>

#include "APG/APGGLRenderTest.hpp"
#include "APG/Game.hpp"
#include "APG/SDLGame.hpp"
#include "APG/APGCommon.hpp"
#include "APG/SXXDL.hpp"
#include "APG/Buffer.hpp"
#include "APG/Texture.hpp"

const char *APG::APGGLRenderTest::vertexShaderFilename = "assets/pass_vertex.glslv";
const char * APG::APGGLRenderTest::fragmentShaderFilename = "assets/red_frag.glslf";

bool APG::APGGLRenderTest::init() {
	if (hasError()) {
		std::cerr << "Failed to initialise APGGLRenderTest.\n";
		return false;
	}

	map = std::make_unique<Tmx::Map>();
	map->ParseFile("assets/world1.tmx");

	if (map->HasError()) {
		std::cerr << "Error loading map: " << map->GetErrorText() << std::endl;
		return false;
	}

	shaderProgram = ShaderProgram::fromFiles(vertexShaderFilename, fragmentShaderFilename);

	if (shaderProgram->hasError()) {
		std::cout << "Shader Info Log\n---------------\n" << shaderProgram->getShaderInfoLog();
		std::cout << "\nLink Info Log\n-------------\n" << shaderProgram->getLinkInfoLog()
				<< std::endl;
		std::cerr << "Couldn't create shader:\n" << shaderProgram->getErrorMessage() << std::endl;
		return false;
	}

	spriteBatch = std::make_unique<SpriteBatch>(SpriteBatch::DEFAULT_BUFFER_SIZE);

	if (spriteBatch->hasError()) {
		std::cout << "Couldn't create sprite batch:\n" << spriteBatch->getErrorMessage()
				<< std::endl;
		return false;
	}

	renderer = std::make_unique<GLTmxRenderer>(map.get(), *spriteBatch);

	if (renderer->hasError() || renderer->getTilesetByID(0) == nullptr) {
		std::cout << "Couldn't create GL tmx renderer:\n" << renderer->getErrorMessage()
				<< std::endl;
		return false;
	}

	sprite = std::make_unique<Sprite>(renderer->getTilesetByID(0), 0, 0, 32, 32);

	if (sprite->hasError()) {
		std::cout << "Couldn't create sprite:\n" << sprite->getErrorMessage() << std::endl;
		return false;
	}

	auto glError = glGetError();
	if (glError != GL_NO_ERROR) {
		while (glError != GL_NO_ERROR) {
			std::cerr << "Error in gl: " << gluErrorString(glError) << std::endl;
			glError = glGetError();
		}

		return false;
	}

	return true;
}

void APG::APGGLRenderTest::render(float deltaTime) {
	glClearColor(0.313725f, 0.674510f, 0.239216f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (spriteBatch->hasError()) {
		std::cerr << "Error in SpriteBatch:\n" << spriteBatch->getErrorMessage() << std::endl;
		quit();
		return;
	}

//	spriteBatch->begin();
//
//	for (int y = 0; y < 32 * 100; y += 32) {
//		for (int x = 0; x < 32 * 100; x += 32) {
//			spriteBatch->draw(sprite.get(), x, y);
//		}
//	}
//
//	spriteBatch->end();

	renderer->renderAll();

	SDL_GL_SwapWindow(window.get());
}

#ifndef APG_TEST_SDL
int main(int argc, char *argv[]) {
	APG::SDLGame::sdlWindowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

	auto game = std::make_unique<APG::APGGLRenderTest>("APG GLTmxRenderer Example", 1280, 720);

	if (!game->init()) {
		return EXIT_FAILURE;
	}

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

		done = game->update(deltaTime);

		if (timesTaken.size() >= 100) {
			const float sum = std::accumulate(timesTaken.begin(), timesTaken.end(), 0.0f);

			const float fps = 1 / (sum / timesTaken.size());

			std::cout << "FPS: " << fps << std::endl;

			timesTaken.clear();
		}
	}

	return EXIT_SUCCESS;
}
#endif
