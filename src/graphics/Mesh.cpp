#ifndef APG_NO_GL

#include <cstdint>

#include "APG/graphics/Mesh.hpp"
#include "APG/graphics/VertexAttributeList.hpp"

APG::Mesh::Mesh(std::initializer_list<VertexAttribute> attList) :
		        Mesh(false, false, attList) {
}

APG::Mesh::Mesh(bool staticVertices, bool staticIndices, std::initializer_list<APG::VertexAttribute> attList) :
		        vertexBuffer(staticVertices, attList),
		        indexBuffer(staticIndices) {
}

#endif
