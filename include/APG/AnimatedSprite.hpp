/*
 * APGGLRenderTest.cpp
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

#ifndef ANIMATED_SPRITE__INCLUDE
#define ANIMATED_SPRITE__INCLUDE

#include <cstdint>

#include "Sprite.hpp"

namespace Tmx {
	class Tile;
}

namespace APG {

class AnimatedSprite : public Sprite {
private:
	uint32_t frameCount;

	float secondsPerFrame;
	float animTime;


public:
	AnimatedSprite(Texture * const texture, int32_t texX, int32_t texY, int32_t width, int32_t height, uint32_t frameCount, float secondsPerFrame) :
		Sprite(texture, texX, texY, width, height),
		frameCount(frameCount),
		secondsPerFrame(secondsPerFrame) {
		
	}

	AnimatedSprite(Tmx::Tile *tile);

	void update(float deltaTime);

    int getU() const override;
    int getV() const override;
    int getU2() const override;
    int getV2() const override;

    Sprite *getFrame(int frameNumber) const;
	
};

}

#endif
