/*
 * SpriteBatch.hpp
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

#ifndef SPRITEBATCH_HPP_
#define SPRITEBATCH_HPP_

#include <cstdint>

#include <memory>

#include "ErrorBase.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "ShaderProgram.hpp"

struct SDL_Surface;

namespace APG {

class SpriteBatch : public ErrorBase {
private:
	static const char * const POSITION_ATTRIBUTE;
	static const char * const COLOR_ATTRIBUTE;
	static const char * const TEXCOORD_ATTRIBUTE;

	uint32_t bufferSize = DEFAULT_BUFFER_SIZE;

	std::unique_ptr<APG::ShaderProgram> ownedShaderProgram = std::unique_ptr<APG::ShaderProgram>(
			nullptr);
	ShaderProgram *program = nullptr;

	bool drawing = false;

	Buffer<> vertexBuffer;

public:
	static const uint32_t DEFAULT_BUFFER_SIZE;
	SpriteBatch(uint32_t bufferSize = DEFAULT_BUFFER_SIZE, ShaderProgram * const program = nullptr);
	~SpriteBatch();

	void begin();
	void end();

	void flush();

	void draw(Texture * const image, glm::vec2::type x, glm::vec2::type y, glm::vec2::type width,
			glm::vec2::type height, glm::vec2::type srcX, glm::vec2::type srcY,
			glm::vec2::type srcWidth, glm::vec2::type srcHeight);

	static std::unique_ptr<APG::ShaderProgram> createDefaultShader();
};

}

#endif /* SPRITEBATCH_HPP_ */
