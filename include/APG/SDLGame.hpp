/*
 * SDLGame.hpp
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

#ifndef SDLGAME_HPP_
#define SDLGAME_HPP_

#include <cstdint>

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glew.h>

#include <Game.hpp>
#include <SXXDL.hpp>

namespace APG {

class SDLGame : public Game {
protected:
	SXXDL::window_ptr window = SXXDL::make_window_ptr(nullptr);
	SDL_GLContext glContext = nullptr;

	SDL_Event eventCache;

	std::string windowTitle;

	bool shouldQuit = false;

	void handleEvent(SDL_Event &event);

	void initSDL(uint32_t windowWidth, uint32_t windowHeight);
	void initGL(uint8_t glMajorVersion, uint8_t glMinorVersion);
	void initContextAndGlew();

public:
	SDLGame(const std::string &windowTitle, uint32_t windowWidth, uint32_t windowHeight,
			uint8_t glMajorVersion, uint8_t glMinorVersion);
	virtual ~SDLGame();

	bool update(float deltaTime) override;
	void quit();

	SDL_Window *getWindow() const {
		return window.get();
	}

	SDL_GLContext getGLContext() const {
		return glContext;
	}

	std::string getWindowTitle() const {
		return windowTitle;
	}

	static uint32_t sdlInitFlags;
	static uint32_t sdlImageInitFlags;
	static uint32_t sdlWindowFlags;
};

}

#endif /* SDLGAME_HPP_ */
