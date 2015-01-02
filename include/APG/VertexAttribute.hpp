/*
 * VertexAttribute.hpp
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

#ifndef VERTEXATTRIBUTE_HPP_
#define VERTEXATTRIBUTE_HPP_

#include <cstdint>

#include <string>

namespace APG {

enum class AttributeUsage {
	POSITION, COLOR, TEXCOORD, NORMAL,
};

class VertexAttribute {
private:
	std::string alias;
	AttributeUsage usage;

	uint8_t numComponents = 0;

	bool normalized = false;

public:
	VertexAttribute(const std::string &alias, AttributeUsage usage, uint8_t numComponents, bool normalized = false);
};

}

#endif /* VERTEXATTRIBUTE_HPP_ */
