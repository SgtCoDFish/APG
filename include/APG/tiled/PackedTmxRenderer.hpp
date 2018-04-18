#ifndef APG_TILED_PACKEDTMXRENDERER_HPP
#define APG_TILED_PACKEDTMXRENDERER_HPP

#include <string>
#include <vector>
#include <memory>

#include "Tmx.h"

#include "APG/graphics/Sprite.hpp"
#include "APG/graphics/PackedTexture.hpp"

namespace Tmx {
class Tile;
}

namespace APG {

class PackedTmxRenderer final {
public:
	explicit PackedTmxRenderer(const std::string &filename);
	~PackedTmxRenderer() = default;

private:
	std::unique_ptr<Tmx::Map> map;
	PackedTexture packedTexture;

	std::unordered_map<uint64_t, SpriteBase *> sprites;
	std::vector<Sprite> loadedSprites;
};

}

#endif
