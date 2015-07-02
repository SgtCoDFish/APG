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

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include "APG/Game.hpp"
#include "APG/SDLGame.hpp"
#include "APG/APGCommon.hpp"
#include "APG/SXXDL.hpp"
#include "APG/Buffer.hpp"
#include "APG/Texture.hpp"

#include "test/APGGLRenderTest.hpp"

const char * APG::APGGLRenderTest::vertexShaderFilename = "assets/pass_vertex.glslv";
const char * APG::APGGLRenderTest::fragmentShaderFilename = "assets/red_frag.glslf";

bool APG::APGGLRenderTest::init() {
	map = std::make_unique<Tmx::Map>();
	map->ParseFile("assets/world1.tmx");

	if (map->HasError()) {
		std::cerr << "Error loading map: " << map->GetErrorText() << std::endl;
		return false;
	}

	shaderProgram = ShaderProgram::fromFiles(vertexShaderFilename, fragmentShaderFilename);

	spriteBatch = std::make_unique<SpriteBatch>();

	renderer = std::make_unique<GLTmxRenderer>(map.get(), *spriteBatch);

	playerTexture = std::make_unique<Texture>("assets/player.png");
	playerFrames = AnimatedSprite::splitTexture(playerTexture.get(), 32, 32, 0, 64, 4);
	playerAnimation = std::make_unique<AnimatedSprite>(0.3f, playerFrames, AnimationMode::LOOP);

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

	renderer->renderAll(deltaTime);
	playerAnimation->update(deltaTime);

	static float playerX = 128.0f, playerY = 128.0f;

	// will move the player quickly
	if (inputManager.isKeyPressed(SDL_SCANCODE_UP)) {
		playerY -= 32.0f;
	} else if (inputManager.isKeyPressed(SDL_SCANCODE_DOWN)) {
		playerY += 32.0f;
	} else if (inputManager.isKeyPressed(SDL_SCANCODE_LEFT)) {
		playerX -= 32.0f;
	} else if (inputManager.isKeyPressed(SDL_SCANCODE_RIGHT)) {
		playerX += 32.0f;
	}

	// will move the player once per press
	if (inputManager.isKeyJustPressed(SDL_SCANCODE_W)) {
		playerY -= 32.0f;
	} else if (inputManager.isKeyJustPressed(SDL_SCANCODE_S)) {
		playerY += 32.0f;
	} else if (inputManager.isKeyJustPressed(SDL_SCANCODE_A)) {
		playerX -= 32.0f;
	} else if (inputManager.isKeyJustPressed(SDL_SCANCODE_D)) {
		playerX += 32.0f;
	}

	spriteBatch->begin();
	spriteBatch->draw(playerAnimation.get(), playerX, playerY);
	spriteBatch->end();

	SDL_GL_SwapWindow(window.get());
}

int main(int argc, char *argv[]) {
	const std::string windowTitle("APG GLTmxRenderer Example");
	const uint32_t windowWidth = 1280;
	const uint32_t windowHeight = 720;

	// reset all errors since apparently glew causes some.
	auto error = glGetError();
	while (error != GL_NO_ERROR) {
		error = glGetError();
	}

	auto game = std::make_unique<APG::APGGLRenderTest>(windowTitle, windowWidth, windowHeight);

	if (!game->init()) {
		std::cerr << "Couldn't initialise game.\n";
		return EXIT_FAILURE;
	}

	bool done = false;

	auto startTime = std::chrono::high_resolution_clock::now();
	std::vector<float> timesTaken;

	while (!done) {
		const auto timeNow = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - startTime).count() / 1000.0f;

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
