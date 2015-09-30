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

#include "APG/SXXDL.hpp"
#include "APG/core/APGeasylogging.hpp"
INITIALIZE_EASYLOGGINGPP
#include "APG/core/Game.hpp"
#include "APG/core/SDLGame.hpp"
#include "APG/core/APGCommon.hpp"
#include "APG/graphics/Buffer.hpp"
#include "APG/graphics/Texture.hpp"
#include "APG/graphics/Camera.hpp"

#include "test/APGGLRenderTest.hpp"

const char * APG::APGGLRenderTest::vertexShaderFilename = "assets/pass_vertex.glslv";
const char * APG::APGGLRenderTest::fragmentShaderFilename = "assets/red_frag.glslf";

bool APG::APGGLRenderTest::init() {
	const auto logger = el::Loggers::getLogger("APG");

	auto map1 = std::make_unique<Tmx::Map>();
	map1->ParseFile("assets/sample_indoor.tmx");

	if (map1->HasError()) {
		logger->fatal("Error loading map1: %v", map1->GetErrorText());
		return false;
	}

	auto map2 = std::make_unique<Tmx::Map>();
	map2->ParseFile("assets/world1.tmx");

	if (map2->HasError()) {
		logger->fatal("Error loading map2: %v", map2->GetErrorText());
		return false;
	}

	shaderProgram = ShaderProgram::fromFiles(vertexShaderFilename, fragmentShaderFilename);

	camera = std::make_unique<Camera>(screenWidth, screenHeight);
	camera->setToOrtho(false, screenWidth, screenHeight);
	spriteBatch = std::make_unique<SpriteBatch>(shaderProgram.get());

	rendererOne = std::make_unique<GLTmxRenderer>(std::move(map1), spriteBatch.get());
	rendererTwo = std::make_unique<GLTmxRenderer>(std::move(map2), spriteBatch.get());
	currentRenderer = rendererOne.get();

	playerTexture = std::make_unique<Texture>("assets/player.png");
	playerFrames = AnimatedSprite::splitTexture(playerTexture, 32, 32, 0, 32, 4);
	playerAnimation = std::make_unique<AnimatedSprite>(0.3f, playerFrames, AnimationMode::LOOP);

	miniTexture = std::make_unique<Texture>("assets/player16.png");
	miniPlayer = std::make_unique<Sprite>(miniTexture);

	currentPlayer = miniPlayer.get();

	font = fontManager->loadFontFile("assets/test_font.ttf", 12);
	const auto renderedFontSize = fontManager->estimateSizeOf(font, "Hello, World!");

	logger->info("Estimated font size: (w, h) = (%v, %v).", renderedFontSize.x, renderedFontSize.y);

	fontSprite = fontManager->renderText(font, "Hello, world!", true, FontRenderMethod::NICE);

	auto glError = glGetError();
	if (glError != GL_NO_ERROR) {
		while (glError != GL_NO_ERROR) {
			logger->fatal("Error in OpenGL while loading: ", gluErrorString(glError));
			glError = glGetError();
		}

		return false;
	}

	return true;
}

void APG::APGGLRenderTest::render(float deltaTime) {
	glClearColor(0.313725f, 0.674510f, 0.239216f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	static float playerX = 128.0f, playerY = 128.0f;
	const glm::vec3 textScreenPosition { 50.0f, screenHeight - 50.0f, 0.0f };
	static glm::vec3 textPos;

	// will move the player quickly
	if (inputManager->isKeyPressed(SDL_SCANCODE_UP)) {
		playerY -= currentPlayer->getHeight();
	} else if (inputManager->isKeyPressed(SDL_SCANCODE_DOWN)) {
		playerY += currentPlayer->getHeight();
	} else if (inputManager->isKeyPressed(SDL_SCANCODE_LEFT)) {
		playerX -= currentPlayer->getWidth();
	} else if (inputManager->isKeyPressed(SDL_SCANCODE_RIGHT)) {
		playerX += currentPlayer->getWidth();
	}

	// will move the player once per press
	if (inputManager->isKeyJustPressed(SDL_SCANCODE_W)) {
		playerY -= currentPlayer->getHeight();
	} else if (inputManager->isKeyJustPressed(SDL_SCANCODE_S)) {
		playerY += currentPlayer->getHeight();
	} else if (inputManager->isKeyJustPressed(SDL_SCANCODE_A)) {
		playerX -= currentPlayer->getWidth();
	} else if (inputManager->isKeyJustPressed(SDL_SCANCODE_D)) {
		playerX += currentPlayer->getWidth();
	}

	if (inputManager->isKeyJustPressed(SDL_SCANCODE_SPACE)) {
		if (currentRenderer == rendererOne.get()) {
			currentRenderer = rendererTwo.get();
			currentPlayer = playerAnimation.get();
		} else {
			currentRenderer = rendererOne.get();
			currentPlayer = miniPlayer.get();
			playerX = 18 * 16;
			playerY = 21 * 16;
		}
	}

	camera->position.x = playerX - screenWidth / 2.0f;
	camera->position.y = playerY - screenHeight / 2.0f;

	camera->update();
	spriteBatch->setProjectionMatrix(camera->combinedMatrix);

	currentRenderer->renderAll(deltaTime);
	playerAnimation->update(deltaTime);

	textPos = camera->unproject(textScreenPosition);

	spriteBatch->begin();
//	spriteBatch->draw(currentPlayer, playerX, playerY);
	// draw a weird quarter version of the player to test out extended draw method
	spriteBatch->draw(currentPlayer->getTexture(), playerX, playerY, currentPlayer->getWidth() * 2,
	        currentPlayer->getHeight() * 2, 0.0f, 0.0f, currentPlayer->getWidth() * 0.5f,
	        currentPlayer->getHeight() * 0.5f);
	spriteBatch->draw(fontSprite, textPos.x, textPos.y);
	spriteBatch->end();

	SDL_GL_SwapWindow(window.get());
}

int main(int argc, char *argv[]) {
	START_EASYLOGGINGPP(argc, argv);

	const std::string windowTitle("APG GLTmxRenderer Example");
	const uint32_t windowWidth = 1280;
	const uint32_t windowHeight = 720;

	auto game = std::make_unique<APG::APGGLRenderTest>(windowTitle, windowWidth, windowHeight);

	if (!game->init()) {
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

		if (timesTaken.size() >= 500) {
			const float sum = std::accumulate(timesTaken.begin(), timesTaken.end(), 0.0f);

			const float fps = 1 / (sum / timesTaken.size());

			el::Loggers::getLogger("APG")->info("FPS: %v", fps);

			timesTaken.clear();
		}
	}

	return EXIT_SUCCESS;
}
