/*
 * Game.hpp
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

#ifndef APGGAME_HPP_
#define APGGAME_HPP_

#include <memory>

#include "ErrorBase.hpp"

namespace APG {

class Game : public ErrorBase {
public:
	Game() {}
	virtual ~Game() {}

	/**
	 * Should carry out initialisation of the game.
	 * @return true if successful, false if an error state has been set.
	 */
	virtual bool init() = 0;

	/**
	 * Called every frame. Should call render.
	 * @param deltaTime the amount of time elapsed since the last frame.
	 * @return true if it's time to quit, false otherwise.
	 */
	virtual bool update(float deltaTime) = 0;

	/**
	 * Should be called in update(float) every frame and render the whole visible screen.
	 * @param deltaTime the amount of time elapsed since the last frame.
	 */
	virtual void render(float deltaTime) = 0;
};

}

#endif /* APGGAME_HPP_ */
