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

#ifndef APG__BUFFER_HPP_
#define APG__BUFFER_HPP_

#include <cstdint>

#include <string>
#include <vector>
#include <array>

#include <GL/glew.h>
#include <GL/glu.h>

#include "easylogging++.h"

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

template<typename T, int glType> class Buffer {
protected:
	uint32_t bufferID;

	const BufferType bufferType;
	const DrawType drawType;

	std::vector<T> bufferData;
	uint64_t elementCount = 0;

	void generateID() {
		glGenBuffers(1, &bufferID);
	}

public:
	explicit Buffer(BufferType bufferType, DrawType drawType) :
			Buffer { bufferType, drawType, nullptr, 0 } {
	}

	explicit Buffer(BufferType bufferType, DrawType drawType, T * const data, uint64_t elementCount) :
			bufferType { bufferType }, drawType { drawType } {
		generateID();
		bind();

		if (data != nullptr) {
			setData(data, elementCount);
			upload();
		}
	}

	virtual ~Buffer() {
		glDeleteBuffers(1, &bufferID);
	}

	void bind() {
		glBindBuffer(bufferType, bufferID);
		upload();
	}

	void upload() {
		// TODO: This should be improved.
		const auto bufferSize = elementCount * sizeof(T);
		glBufferData(bufferType, bufferSize, &(bufferData.front()), drawType);

		GLenum glError = glGetError();
		if (glError != GL_NO_ERROR) {
			if (glError == GL_OUT_OF_MEMORY) {
				el::Loggers::getLogger("default")->error(
				        "Ran out of memory trying to upload buffer data with buffer size = %vB", bufferSize);
				return;
			} else {
				el::Loggers::getLogger("default")->error("OpenGL error occurred: %v.", gluErrorString(glError));
				return;
			}
		}

	}

	const BufferType &getBufferType() const {
		return bufferType;
	}

	DrawType getDrawType() const {
		return drawType;
	}

	/**
	 * Sets the buffer's data to copy <em>data</em>. Removes everything else in the buffer.
	 * @param data
	 * @param elementCount
	 */
	void setData(T * const data, uint64_t elementCount) {
		bufferData.clear();
		for (uint64_t i = 0; i < elementCount; i++) {
			bufferData[i] = data[i];
		}
		this->elementCount = elementCount;
	}

	/**
	 * Sets the buffer's data to copy <em>data</em>. Removes everything else in the buffer.
	 * @param data
	 */
	template<int bufferSize> void setData(std::array<T, bufferSize> &data) {
		bufferData.clear();

		for (int i = 0; i < bufferSize; i++) {
			bufferData[i] = data[i];
		}

		elementCount = data.size();
	}

	void setData(const std::vector<T> &buffer, uint64_t amount = 0) {
		bufferData.clear();

		if (amount == 0) {
			amount = buffer.size();
		}

		for (uint64_t i = 0; i < amount; i++) {
			bufferData.emplace_back(buffer[i]);
		}

		elementCount = amount;
	}

	int getGLType() const {
		return glType;
	}

	uint32_t getBufferID() const {
		return bufferID;
	}

	Buffer(Buffer &other) = delete;
	Buffer(const Buffer &other) = delete;
	Buffer &operator=(Buffer &other) = delete;
	Buffer &operator=(const Buffer &other) = delete;
};

using DoubleBuffer = Buffer<double, GL_DOUBLE>;
using FloatBuffer = Buffer<float, GL_FLOAT>;
using UInt32Buffer = Buffer<uint32_t, GL_UNSIGNED_INT>;
using UInt16Buffer = Buffer<uint16_t, GL_UNSIGNED_SHORT>;
using UInt8Buffer = Buffer<uint8_t, GL_UNSIGNED_BYTE>;
using Int32Buffer = Buffer<int32_t, GL_INT>;
using Int16Buffer = Buffer<int16_t, GL_SHORT>;
using Int8Buffer = Buffer<int8_t, GL_BYTE>;

}

#endif /* APG__BUFFER_HPP_ */
