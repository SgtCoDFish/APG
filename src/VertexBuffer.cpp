/*
 * VertexBuffer.cpp
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

#include "VertexBuffer.hpp"

APG::VertexBuffer::VertexBuffer(BufferType bufferType, DrawType drawType, float * const vertices,
		uint64_t elementCount) :
		bufferType { bufferType }, drawType { drawType } {
	generateID();
	bind();

	if (vertices != nullptr) {
		setVertices(vertices, elementCount);
		upload();
	}
}

void APG::VertexBuffer::generateID() {
	glGenBuffers(1, &vboID);
}

APG::VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &vboID);
}

void APG::VertexBuffer::bind() const {
	glBindBuffer(bufferType, vboID);
}

void APG::VertexBuffer::upload() {
	if (!hasError() && vertices != nullptr && elementCount > 0) {
		bind();
		glBufferData(bufferType, elementCount * sizeof(float), vertices, drawType);

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

void APG::VertexBuffer::setDrawType(DrawType drawType) {
	this->drawType = drawType;
	upload();
}
