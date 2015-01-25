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

#ifndef VERTEXATTRIBUTELIST_HPP_
#define VERTEXATTRIBUTELIST_HPP_

#include <cstdint>

#include <vector>

#include "APG/VertexAttribute.hpp"

namespace APG {

class VertexAttributeList {
private:
	std::vector<VertexAttribute> attributes;

	uint16_t stride = 0;
	void calculateOffsets();

public:
	explicit VertexAttributeList(std::initializer_list<VertexAttribute> initList);
	explicit VertexAttributeList(std::vector<VertexAttribute> &attVec);

	/**
	 * @param attribute the attribute to be copied into this list.
	 */
	void addAttribute(const VertexAttribute &attribute);

	/**
	 * @param attribute the attribute to be moved into this list.
	 */
	void addAttribute(VertexAttribute &&attribute);

	inline uint16_t getStride() const {
		return stride;
	}

	inline int getAttributeCount() const {
		return attributes.size();
	}

	inline const std::vector<VertexAttribute> &getAttributes() const {
		return attributes;
	}

	const VertexAttribute &operator[](unsigned int index) const {
		return attributes[index];
	}
};

}

#endif /* VERTEXATTRIBUTELIST_HPP_ */
