#ifndef INCLUDE_APG_GRAPHICS_BUFFER_HPP_
#define INCLUDE_APG_GRAPHICS_BUFFER_HPP_

#ifndef APG_NO_GL

#include <cstdint>

#include <string>
#include <vector>
#include <array>

#include "spdlog/spdlog.h"

#include "APG/GL.hpp"

#include "APG/graphics/GLError.hpp"

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
public:
	explicit Buffer(BufferType bufferType, DrawType drawType) :
			        Buffer { bufferType, drawType, nullptr, 0 } {
	}

	explicit Buffer(BufferType bufferType, DrawType drawType, T * const data, uint64_t elementCount) :
			        bufferType { bufferType },
			        drawType { drawType },
					logger{spdlog::get("APG")} {
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
		glBufferData(bufferType, bufferSize, bufferData.data(), drawType);

		GLenum glError = glGetError();
		if (glError != GL_NO_ERROR) {
			if (glError == GL_OUT_OF_MEMORY) {
				logger->error(
				        "Ran out of memory trying to upload buffer data with buffer size = {}B", bufferSize);
				return;
			} else {
				logger->error("OpenGL error occurred: {}.", prettyGLError(glError));
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

protected:
	uint32_t bufferID;

	const BufferType bufferType;
	const DrawType drawType;

	std::vector<T> bufferData;
	uint64_t elementCount = 0;

	void generateID() {
		glGenBuffers(1, &bufferID);
	}

private:
	std::shared_ptr<spdlog::logger> logger;
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

#endif

#endif