/*
 * SDLGame.cpp
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

#include <cstdint>

#include <sstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>

#include <SDLGame.hpp>

uint32_t APG::SDLGame::sdlInitFlags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
uint32_t APG::SDLGame::sdlImageInitFlags = IMG_INIT_PNG;
uint32_t APG::SDLGame::sdlWindowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

APG::SDLGame::SDLGame(const std::string &windowTitle, uint32_t windowWidth, uint32_t windowHeight,
		uint8_t glMajorVersion, uint8_t glMinorVersion) :
		windowTitle(std::string(windowTitle)) {
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
	initGlew();
}

APG::SDLGame::~SDLGame() {
	if(glContext != nullptr) {
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

void APG::SDLGame::initGlew() {
	glContext = SDL_GL_CreateContext(window.get());
	glewExperimental = GL_TRUE;
	glewInit();
}

void APG::SDLGame::quit() {
	shouldQuit = true;
}
