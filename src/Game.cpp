/*
 * Game.cpp
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

#include <iostream>
#include <sstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Game.hpp"

APG::Game::Game() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		std::stringstream ss;
		ss << "Couldn't initialise SDL2:\n" << SDL_GetError();
		setErrorState(ss.str());
		return;
	}

	const int imageFlags = IMG_INIT_PNG;
	if ((IMG_Init(imageFlags) & imageFlags) == 0) {
		std::stringstream ss;
		ss << "Couldn't initialise SDL2_image:\n" << IMG_GetError() << std::endl;
		setErrorState(ss.str());
		return;
	}
}

APG::Game::~Game() {
	IMG_Quit();
	SDL_Quit();
}
