/*
 * VertexBufferObject.hpp
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

#ifndef VERTEXBUFFEROBJECT_HPP_
#define VERTEXBUFFEROBJECT_HPP_

#include <cstdint>

#include "Buffer.hpp"
#include "VertexAttributeList.hpp"
#include "ShaderProgram.hpp"

namespace APG {

/**
 * A float buffer for vertices with vertex attributes added in for good measure.
 */
class VertexBufferObject : public FloatBuffer {
private:
	VertexAttributeList attributeList;

public:
	explicit VertexBufferObject(std::initializer_list<VertexAttribute> initList);
	VertexBufferObject(bool isStatic, std::initializer_list<VertexAttribute> initList);
	VertexBufferObject(bool isStatic, std::initializer_list<VertexAttribute> initList,
			float vertices[], int vertexCount);

	inline const VertexAttributeList &getAttributes() const {
		return attributeList;
	}

	void bind(ShaderProgram * const program);
};

}

#endif /* VERTEXBUFFEROBJECT_HPP_ */
