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

#include <string>
#include <sstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "APG/SDLGame.hpp"

uint32_t APG::SDLGame::sdlInitFlags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
uint32_t APG::SDLGame::sdlImageInitFlags = IMG_INIT_PNG;
uint32_t APG::SDLGame::sdlWindowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

APG::SDLGame::SDLGame(const std::string &windowTitle, uint32_t windowWidth, uint32_t windowHeight,
		uint8_t glMajorVersion, uint8_t glMinorVersion) :
		Game(windowWidth, windowHeight), windowTitle(std::string(windowTitle)) {
	if (SDL_Init(sdlInitFlags) < 0) {
		setErrorState((std::string("Couldn't initialise SDL: ") + SDL_GetError()));
		return;
	}

	if ((IMG_Init(sdlImageInitFlags) & sdlImageInitFlags) == 0) {
		setErrorState(std::string("Couldn't initialise SDL_image: ") + IMG_GetError());
		return;
	}

	initGL(glMajorVersion, glMinorVersion);
	initSDL(windowWidth, windowHeight);
	initContextAndGlew();
}

APG::SDLGame::~SDLGame() {
	if (glContext != nullptr) {
		SDL_GL_DeleteContext(glContext);
	}

	IMG_Quit();
	SDL_Quit();
}

void APG::SDLGame::initSDL(uint32_t windowWidth, uint32_t windowHeight) {
	if (hasError()) {
		return;
	}

	window = SXXDL::make_window_ptr(
			SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
					windowWidth, windowHeight, sdlWindowFlags));
}

void APG::SDLGame::handleEvent(SDL_Event &event) {
	if (event.type == SDL_QUIT) {
		quit();
	}
}

bool APG::SDLGame::update(float deltaTime) {
	if (shouldQuit) {
		return true;
	}

	while (SDL_PollEvent(&eventCache)) {
		handleEvent(eventCache);
	}

	if (hasError()) {
		std::stringstream ss;
		ss << "Error in SDLGame: " << getErrorMessage() << "\n";
		setErrorState(ss.str().c_str());
		quit();
		return false;
	}

	render(deltaTime);

	return false;
}

void APG::SDLGame::initGL(uint8_t glMajorVersion, uint8_t glMinorVersion) {
	if (hasError()) {
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glMajorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glMinorVersion);
}

void APG::SDLGame::initContextAndGlew() {
	if (hasError()) {
		return;
	}

	glContext = SDL_GL_CreateContext(window.get());

	glewExperimental = GL_TRUE;
	glewInit();

	// reset all errors since apparently glew causes some.
	auto error = glGetError();
	while (error != GL_NO_ERROR) {
		error = glGetError();
	}
}

void APG::SDLGame::quit() {
	shouldQuit = true;
}
