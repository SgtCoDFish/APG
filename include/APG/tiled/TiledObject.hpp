#ifndef APG_TILED_TILEDOBJECT_HPP
#define APG_TILED_TILEDOBJECT_HPP

#include <string>
#include <utility>
#include <glm/vec2.hpp>

namespace APG {
class SpriteBase;

class TiledObject {
public:
	explicit TiledObject(std::string name, const glm::vec2 &pos, SpriteBase *const sprite) :
			name{std::move(name)},
			position{pos},
			sprite{sprite} {
	}

	explicit TiledObject(std::string name, float x, float y, SpriteBase *const sprite) :
			name{std::move(name)},
			position{x, y},
			sprite{sprite} {
	}

	std::string name;
	glm::vec2 position;
	SpriteBase *sprite;
};

}

#endif
