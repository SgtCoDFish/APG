#ifndef APG_TILED_TILEDOBJECT_HPP
#define APG_TILED_TILEDOBJECT_HPP

#include <glm/vec2.hpp>

namespace APG {
class SpriteBase;

class TiledObject {
public:
	explicit TiledObject(const glm::vec2 &pos, SpriteBase *const sprite) :
			position{pos},
			sprite{sprite} {
	}

	explicit TiledObject(float x, float y, SpriteBase *const sprite) :
			position{x, y},
			sprite{sprite} {
	}

	glm::vec2 position;
	SpriteBase *sprite;
};

}

#endif
