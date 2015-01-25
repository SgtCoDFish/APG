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

#ifndef APGGAME_HPP_
#define APGGAME_HPP_

#include <cstdint>

#include "APG/ErrorBase.hpp"

namespace APG {

class Game : public ErrorBase {
public:
	static uint32_t screenWidth;
	static uint32_t screenHeight;

	Game(uint32_t screenWidth = 1280, uint32_t screenHeight = 720) {
		Game::screenWidth = screenWidth;
		Game::screenHeight = screenHeight;
	}

	virtual ~Game() {
	}

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
