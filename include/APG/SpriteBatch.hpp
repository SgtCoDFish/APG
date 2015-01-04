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
#include "VAO.hpp"
#include "Mesh.hpp"
#include "VertexBufferObject.hpp"
#include "IndexBufferObject.hpp"

namespace APG {

class Sprite;

class SpriteBatch : public ErrorBase {
private:
	static const char * const POSITION_ATTRIBUTE;
	static const char * const COLOR_ATTRIBUTE;
	static const char * const TEXCOORD_ATTRIBUTE;

	const uint32_t bufferSize;

	bool drawing = false;

	VAO vao;
	VertexBufferObject vertexBuffer;
	IndexBufferObject indexBuffer;

	std::vector<float> vertices;

	std::unique_ptr<APG::ShaderProgram> ownedShaderProgram;
	ShaderProgram *program = nullptr;

	uint64_t idX = 0;

public:
	static const uint32_t DEFAULT_BUFFER_SIZE;
	explicit SpriteBatch(uint32_t bufferSize = DEFAULT_BUFFER_SIZE, ShaderProgram * const program =
			nullptr);
	~SpriteBatch();

	void begin();
	void end();

	void flush();

	void draw(Texture * const image, float x, float y, uint32_t width, uint32_t height, float srcX,
			float srcY, uint32_t srcWidth, uint32_t srcHeight);

	void draw(Sprite * const sprite, float x, float y);

	static std::unique_ptr<APG::ShaderProgram> createDefaultShader();
};

}

#endif /* SPRITEBATCH_HPP_ */
