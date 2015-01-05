/*
 * VertexBufferObject.cpp
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

#include <cstdint>

#include "Buffer.hpp"
#include "VertexBufferObject.hpp"
#include "VertexAttribute.hpp"
#include "ShaderProgram.hpp"

APG::VertexBufferObject::VertexBufferObject(std::initializer_list<APG::VertexAttribute> initList) :
		VertexBufferObject(false, initList) {
}

APG::VertexBufferObject::VertexBufferObject(bool isStatic,
		std::initializer_list<APG::VertexAttribute> initList) :
		VertexBufferObject(isStatic, initList, nullptr, 0) {
}

APG::VertexBufferObject::VertexBufferObject(bool isStatic,
		std::initializer_list<VertexAttribute> initList, float vertices[], int vertexCount) :
		APG::FloatBuffer(BufferType::ARRAY,
				isStatic ? DrawType::STATIC_DRAW : DrawType::DYNAMIC_DRAW, vertices, vertexCount), //
		attributeList { initList } {

}

void APG::VertexBufferObject::bind(ShaderProgram * const program) {
	Buffer::bind();

	program->setVertexAttributes(attributeList);
}
