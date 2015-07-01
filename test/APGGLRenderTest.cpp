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

#include "APG/APGGLRenderTest.hpp"
#include "APG/Game.hpp"
#include "APG/SDLGame.hpp"
#include "APG/APGCommon.hpp"
#include "APG/SXXDL.hpp"
#include "APG/Buffer.hpp"
#include "APG/Texture.hpp"

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

	SDL_GL_SwapWindow(window);
}

#ifndef APG_TEST_SDL
int main(int argc, char *argv[]) {
	uint32_t sdlInitFlags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
	uint32_t sdlImageInitFlags = IMG_INIT_PNG;
	uint32_t sdlWindowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

	if (SDL_Init(sdlInitFlags) < 0) {
		std::cerr << "Couldn't initialise SDL: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	if ((IMG_Init(sdlImageInitFlags) & sdlImageInitFlags) == 0) {
		std::cerr << "Couldn't initialise SDL_image: " << IMG_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	const std::string windowTitle("APG GLTmxRenderer Example");
	const uint32_t windowWidth = 1280;
	const uint32_t windowHeight = 720;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	SDL_Window *window = SDL_CreateWindow(windowTitle.c_str(), 0, 0, windowWidth, windowHeight, sdlWindowFlags);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	glewInit();

	// reset all errors since apparently glew causes some.
	auto error = glGetError();
	while (error != GL_NO_ERROR) {
		error = glGetError();
	}

	{
		auto game = std::make_unique<APG::APGGLRenderTest>(window, 1280, 720, context);

		if (!game->init()) {
			std::cerr << "Couldn't initialise game.\n";
			return EXIT_FAILURE;
		}

		bool done = false;

		auto startTime = std::chrono::high_resolution_clock::now();
		std::vector<float> timesTaken;

		while (!done) {
			const auto timeNow = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - startTime).count()
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
		//
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);

	IMG_Quit();
	SDL_Quit();

	return EXIT_SUCCESS;
}
#endif
