/*
 * APGGLRenderTest.cpp
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

#include <cstdint>
#include <cstdlib>

#include <iostream>
#include <memory>
#include <chrono>
#include <vector>
#include <numeric>

#include <GL/glew.h>
#include <GL/gl.h>

#include "APGGLRenderTest.hpp"
#include "Game.hpp"
#include "SDLGame.hpp"
#include "APGCommon.hpp"
#include "SXXDL.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"

const char *APG::APGGLRenderTest::vertexShaderFilename = "pass_vertex.glslv";
const char * APG::APGGLRenderTest::fragmentShaderFilename = "red_frag.glslf";

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

	if (renderer->hasError()) {
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

	spriteBatch->begin();

	spriteBatch->draw(sprite.get(), 0, 0);

	spriteBatch->end();

	SDL_GL_SwapWindow(window.get());
}

#ifndef APG_TEST_SDL
int main(int argc, char *argv[]) {
	APG::SDLGame::sdlWindowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
	auto game = std::make_unique<APG::APGGLRenderTest>();

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

		if (timesTaken.size() >= 1000) {
			const float sum = std::accumulate(timesTaken.begin(), timesTaken.end(), 0.0f);

			const float fps = 1 / (sum / timesTaken.size());

			std::cout << "FPS: " << fps << std::endl;

			timesTaken.clear();
		}
	}

	return EXIT_SUCCESS;
}
#endif
