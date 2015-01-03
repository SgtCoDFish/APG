/*
 * VertexAttributeList.hpp
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

#ifndef VERTEXATTRIBUTELIST_HPP_
#define VERTEXATTRIBUTELIST_HPP_

#include <cstdint>

#include <vector>

#include "VertexAttribute.hpp"

namespace APG {

class VertexAttributeList {
private:
	std::vector<VertexAttribute> attributes;

	uint16_t stride = 0;
	void calculateOffsets();

public:
	explicit VertexAttributeList(std::initializer_list<VertexAttribute> initList) {
		for (const auto &att : initList) {
			attributes.emplace_back(VertexAttribute(att));
		}

		calculateOffsets();
	}
};

}

#endif /* VERTEXATTRIBUTELIST_HPP_ */
