/*
 * Buffer.hpp
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

#ifndef APG__BUFFER_HPP_
#define APG__BUFFER_HPP_

#include <cstdint>

#include <sstream>
#include <string>

#include <GL/glew.h>
#include <GL/glu.h>

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

template<typename T = float, int glType = GL_FLOAT> class Buffer : public ErrorBase {
private:
	uint32_t bufferID;

	const BufferType bufferType;
	DrawType drawType;

	T *data = nullptr;
	uint64_t elementCount = 0;

	void generateID() {
		glGenBuffers(1, &bufferID);
	}

public:
	Buffer(BufferType bufferType, DrawType drawType) :
			Buffer { bufferType, drawType, nullptr, 0 } {
	}

	Buffer(BufferType bufferType, DrawType drawType, T * const data, uint64_t elementCount) :
			bufferType { bufferType }, drawType { drawType } {
		generateID();
		bind();

		if (data != nullptr) {
			setData(data, elementCount);
			upload();
		}
	}

	~Buffer() {
		glDeleteBuffers(1, &bufferID);
	}

	void bind() const {
		glBindBuffer(bufferType, bufferID);
	}

	void upload() {
		if (!hasError() && data != nullptr && elementCount > 0) {
			bind();
			glBufferData(bufferType, elementCount * sizeof(T), data, drawType);

			GLenum glError = glGetError();
			if (glError != GL_NO_ERROR) {
				if (glError == GL_OUT_OF_MEMORY) {
					setErrorState("Ran out of memory trying to upload vertex data.");
					return;
				} else {
					std::stringstream ss;
					ss << "OpenGL error:\n" << gluErrorString(glError);
					setErrorState(ss.str());
					return;
				}
			}
		}
	}

	void setDrawType(DrawType drawType) {
		this->drawType = drawType;
		upload();
	}

	const BufferType &getBufferType() const {
		return bufferType;
	}

	DrawType getDrawType() const {
		return drawType;
	}

	void setData(T * const data, uint64_t elementCount) {
		this->data = data;
		this->elementCount = elementCount;
	}

	int getGLType() const {
		return glType;
	}

	Buffer(Buffer &other) = delete;
	Buffer(const Buffer &other) = delete;
	Buffer &operator=(Buffer &other) = delete;
	Buffer &operator=(const Buffer &other) = delete;
};

}

#endif /* APG__BUFFER_HPP_ */
