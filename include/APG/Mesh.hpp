/*
 * Mesh.hpp
 * Copyright (C) 2014, 2015 Ashley Davis (SgtCoDFish)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hopesthat it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef MESH_HPP_
#define MESH_HPP_

#include <cstdint>

#include <memory>

#include "VertexAttributeList.hpp"
#include "VertexBufferObject.hpp"
#include "IndexBufferObject.hpp"

namespace APG {

class Mesh {
private:
	std::unique_ptr<VertexAttributeList> attributes = std::unique_ptr<VertexAttributeList>(nullptr);

	std::unique_ptr<VertexBufferObject> vertexBuffer = std::unique_ptr<VertexBufferObject>(nullptr);
	std::unique_ptr<IndexBufferObject> indexBuffer = std::unique_ptr<IndexBufferObject>(nullptr);
public:
	Mesh(std::initializer_list<VertexAttribute> attList);
	Mesh(bool staticVertices, bool staticIndices, std::initializer_list<VertexAttribute> attList);
};

}

#endif /* MESH_HPP_ */
