/*
 * Buffer.cpp
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

#include <cstdint>

#include <sstream>
#include <string>

#include <GL/glew.h>
#include <GL/glu.h>

#include "Buffer.hpp"

APG::Buffer::Buffer(BufferType bufferType, DrawType drawType, float * const data,
		uint64_t elementCount) :
		bufferType { bufferType }, drawType { drawType } {
	generateID();
	bind();

	if (vertices != nullptr) {
		setData(data, elementCount);
		upload();
	}
}

void APG::Buffer::generateID() {
	glGenBuffers(1, &bufferID);
}

APG::Buffer::~Buffer() {
	glDeleteBuffers(1, &bufferID);
}

void APG::Buffer::bind() const {
	glBindBuffer(bufferType, bufferID);
}

void APG::Buffer::upload() {
	if (!hasError() && data != nullptr && elementCount > 0) {
		bind();
		glBufferData(bufferType, elementCount * sizeof(float), data, drawType);

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

void APG::Buffer::setDrawType(DrawType drawType) {
	this->drawType = drawType;
	upload();
}
