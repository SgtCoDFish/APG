#ifndef APG_GRAPHICS_PACKEDTEXTURE_HPP
#define APG_GRAPHICS_PACKEDTEXTURE_HPP

#include <cstdint>
#include "APG/graphics/Texture.hpp"

namespace APG {

class PackTreeNode {
};

class PackedTexture final : public Texture {
public:
	explicit PackedTexture(int32_t width, int32_t height);

	void beginPack();

	void endPack();

private:
	PackTreeNode packTree;
	bool packing{false};
};

}

#endif
