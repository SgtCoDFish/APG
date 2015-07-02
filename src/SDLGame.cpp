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
#include "APG/SDLInputManager.hpp"

#include "APG/internal/Log.hpp"

uint32_t APG::SDLGame::SDL_INIT_FLAGS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS;
uint32_t APG::SDLGame::SDL_IMAGE_INIT_FLAGS = IMG_INIT_PNG;
uint32_t APG::SDLGame::SDL_WINDOW_FLAGS = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

APG::SDLGame::SDLGame(const std::string &windowTitle, uint32_t windowWidth, uint32_t windowHeight,
        uint8_t glContextMajor, uint8_t glContextMinor) :
		APG::Game(windowWidth, windowHeight) {
	if (SDL_Init(SDL_INIT_FLAGS) < 0) {
		std::stringstream ss;
		ss << "Couldn't initialise SDL: " << SDL_GetError() << std::endl;
		APG_LOG(ss.str().c_str());
	}

	if ((IMG_Init(SDL_IMAGE_INIT_FLAGS) & SDL_IMAGE_INIT_FLAGS) == 0) {
		std::stringstream ss;
		ss << "Couldn't initialise SDL_image: " << IMG_GetError() << std::endl;
		APG_LOG(ss.str().c_str());
	}

	window = SXXDL::make_window_ptr(
	        SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth,
	                windowHeight, SDL_WINDOW_FLAGS));

	if (window == nullptr) {
		std::stringstream ss;
		ss << "Couldn't create window: " << SDL_GetError() << std::endl;
		APG_LOG(ss.str().c_str());
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glContextMajor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glContextMinor);

	glContext = SDL_GL_CreateContext(window.get());

	glewExperimental = GL_TRUE;
	glewInit();

	// reset all errors because some are caused by glew even upon successful setup.
	auto error = glGetError();
	while (error != GL_NO_ERROR) {
		error = glGetError();
	}
}

APG::SDLGame::~SDLGame() {
	SDL_GL_DeleteContext(glContext);
	IMG_Quit();
	SDL_Quit();
}

void APG::SDLGame::handleEvent(SDL_Event &event) {
	if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
		inputManager.handleInputEvent(event);
	} else if (event.type == SDL_QUIT) {
		quit();
	}
}

bool APG::SDLGame::update(float deltaTime) {
	if (shouldQuit) {
		return true;
	}

	inputManager.update(deltaTime);

	while (SDL_PollEvent(&eventCache)) {
		handleEvent(eventCache);
	}

	render(deltaTime);

	return false;
}

void APG::SDLGame::quit() {
	shouldQuit = true;
}
