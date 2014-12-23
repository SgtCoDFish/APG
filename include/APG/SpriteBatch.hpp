/*
 * SpriteBatch.hpp
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

#ifndef SPRITEBATCH_HPP_
#define SPRITEBATCH_HPP_

#include <cstdint>

#include "ErrorBase.hpp"

struct SDL_Surface;

namespace APG {

class ShaderProgram;

class SpriteBatch : public ErrorBase {
private:
	static const constexpr uint32_t DEFAULT_BUFFER_SIZE = 1000;

	uint32_t bufferSize = DEFAULT_BUFFER_SIZE;
	ShaderProgram *shaderProgram = nullptr;

	bool drawing = false;

public:
	SpriteBatch(uint32_t bufferSize = DEFAULT_BUFFER_SIZE, ShaderProgram * const program = nullptr);
	~SpriteBatch();

	void begin();
	void end();

	void flush();

	void draw(SDL_Surface * const image, glm::vec2::type x, glm::vec2::type y,
			glm::vec2::type width, glm::vec2::type height, glm::vec2::type srcX,
			glm::vec2::type srcY, glm::vec2::type srcWidth, glm::vec2::type srcHeight);
};

}

#endif /* SPRITEBATCH_HPP_ */
