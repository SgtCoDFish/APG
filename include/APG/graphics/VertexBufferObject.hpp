/*
 * Copyright (c) 2014, 2015 See AUTHORS file.
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

#ifndef INCLUDE_APG_GRAPHICS_VERTEXBUFFEROBJECT_HPP_
#define INCLUDE_APG_GRAPHICS_VERTEXBUFFEROBJECT_HPP_

#include <cstdint>

#include "APG/graphics/Buffer.hpp"
#include "APG/graphics/VertexAttributeList.hpp"
#include "APG/graphics/ShaderProgram.hpp"

namespace APG {

/**
 * A float buffer for vertices with vertex attributes added in for good measure.
 */
class VertexBufferObject : public FloatBuffer {
public:
	explicit VertexBufferObject(std::initializer_list<VertexAttribute> initList);
	explicit VertexBufferObject(bool isStatic, std::initializer_list<VertexAttribute> initList);
	explicit VertexBufferObject(bool isStatic, std::initializer_list<VertexAttribute> initList, float vertices[],
	        int vertexCount);
	virtual ~VertexBufferObject() = default;

	inline const VertexAttributeList &getAttributes() const {
		return attributeList;
	}

	void bind(ShaderProgram * const program);

private:
	VertexAttributeList attributeList;
};

}

#endif /* VERTEXBUFFEROBJECT_HPP_ */
