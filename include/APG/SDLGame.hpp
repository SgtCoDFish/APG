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

#ifndef SDLGAME_HPP_
#define SDLGAME_HPP_

#include <cstdint>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glew.h>

#include "APG/Game.hpp"
#include "APG/SXXDL.hpp"
#include "APG/SDLInputManager.hpp"

namespace APG {

class SDLGame: public Game {
protected:
	SXXDL::window_ptr window = SXXDL::make_window_ptr(nullptr);
	SDL_GLContext glContext = nullptr;

	SDL_Event eventCache;

	bool shouldQuit = false;

	virtual void handleEvent(SDL_Event &event);

	SDLInputManager inputManager;

public:
	/**
	 * Defaults to SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS
	 */
	static uint32_t SDL_INIT_FLAGS;

	/**
	 * Defaults to IMG_INIT_PNG.
	 */
	static uint32_t SDL_IMAGE_INIT_FLAGS;

	/**
	 * Defaults to SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL.
	 */
	static uint32_t SDL_WINDOW_FLAGS;

	explicit SDLGame(const std::string &windowTitle, uint32_t windowWidth, uint32_t windowHeight,
	        uint8_t glContextMajor = 3, uint8_t glContextMinor = 2);
	virtual ~SDLGame() = default;

	bool update(float deltaTime) override;
	void quit();

	SDL_Window *getWindow() const {
		return window.get();
	}

	SDL_GLContext getGLContext() const {
		return glContext;
	}

	virtual const InputManager *input() const override {
		return &inputManager;
	}
};

}

#endif /* SDLGAME_HPP_ */
