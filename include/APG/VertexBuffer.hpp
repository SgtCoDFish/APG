/*
 * VertexBuffer.hpp
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

#ifndef VERTEXBUFFER_HPP_
#define VERTEXBUFFER_HPP_

#include <cstdint>

#include <GL/glew.h>

#include "ErrorBase.hpp"

namespace APG {

enum BufferType {
	ARRAY = GL_ARRAY_BUFFER,
	ELEMENT_ARRAY = GL_ELEMENT_ARRAY_BUFFER,
	PIXEL_PACK = GL_PIXEL_PACK_BUFFER,
	PIXEL_UNPACK = GL_PIXEL_UNPACK_BUFFER,
	UNIFORM = GL_UNIFORM_BUFFER,
	TEXTURE = GL_TEXTURE_BUFFER,
	TRANSFORM_FEEDBACK = GL_TRANSFORM_FEEDBACK,
	COPY_READ = GL_COPY_READ_BUFFER,
	COPY_WRITE = GL_COPY_WRITE_BUFFER,
};

enum DrawType {
	STATIC_DRAW = GL_STATIC_DRAW, STATIC_READ = GL_STATIC_READ, STATIC_COPY = GL_STATIC_COPY,

	STREAM_DRAW = GL_STREAM_DRAW, STREAM_READ = GL_STREAM_READ, STREAM_COPY = GL_STREAM_COPY,

	DYNAMIC_DRAW = GL_DYNAMIC_DRAW, DYNAMIC_READ = GL_DYNAMIC_READ, DYNAMIC_COPY = GL_DYNAMIC_COPY,
};

class VertexBuffer : public ErrorBase {
private:
	uint32_t vboID;

	const BufferType bufferType;
	DrawType drawType;

	float *vertices = nullptr;
	uint64_t elementCount = 0;

	void generateID();

public:
	VertexBuffer(BufferType bufferType, DrawType drawType) :
			VertexBuffer { bufferType, drawType, nullptr, 0 } {
	}

	VertexBuffer(BufferType bufferType, DrawType drawType, float * const vertices,
			uint64_t elementCount);
	~VertexBuffer();

	void bind() const;
	void upload();
	void setDrawType(DrawType drawType);

	const BufferType &getBufferType() const {
		return bufferType;
	}

	DrawType getDrawType() const {
		return drawType;
	}

	void setVertices(float * const vertices, uint64_t elementCount) {
		this->vertices = vertices;
		this->elementCount = elementCount;
	}

	VertexBuffer(VertexBuffer &other) = delete;
	VertexBuffer(const VertexBuffer &other) = delete;
	VertexBuffer &operator=(VertexBuffer &other) = delete;
	VertexBuffer &operator=(const VertexBuffer &other) = delete;
};

}

#endif /* VERTEXBUFFER_HPP_ */
