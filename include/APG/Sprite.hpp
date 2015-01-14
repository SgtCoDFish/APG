/*
 * Sprite.hpp
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

#ifndef SPRITE_HPP_
#define SPRITE_HPP_

#include <cstdint>

#include <memory>

#include "Buffer.hpp"
#include "ErrorBase.hpp"

namespace APG {

class Texture;

class Sprite : public ErrorBase {
protected:
	Texture * texture = nullptr;

	int32_t texX = 0, texY = 0;

	int32_t width = 0, height = 0;

	float u1 = 0.0f, v1 = 0.0f;
	float u2 = 0.0f, v2 = 0.0f;

	void calculateUV();

public:
	Sprite(Texture * const texture, int32_t texX, int32_t texY, int32_t width, int32_t height);

	Texture * const getTexture() const {
		return texture;
	}

	inline int32_t getWidth() const {
		return width;
	}

	inline int32_t getHeight() const {
		return height;
	}

	inline float getU() const {
		return u1;
	}

	inline float getV() const {
		return v1;
	}

	inline float getU2() const {
		return u2;
	}

	inline float getV2() const {
		return v2;
	}

	Sprite(Sprite &other) = delete;
	Sprite(const Sprite &other) = delete;

	Sprite &operator=(Sprite &other) = delete;
	Sprite &operator=(const Sprite &other) = delete;
};

}

#endif /* SPRITE_HPP_ */
