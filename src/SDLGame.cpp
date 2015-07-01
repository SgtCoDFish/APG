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

APG::SDLGame::SDLGame(SDL_Window *window, SDL_GLContext &context, uint32_t windowWidth, uint32_t windowHeight) :
		Game(windowWidth, windowHeight) {

	setWindow(window);
	setGLContext(context);
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

void APG::SDLGame::quit() {
	shouldQuit = true;
}
