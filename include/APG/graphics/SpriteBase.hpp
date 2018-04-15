#ifndef INCLUDE_APG_GRAPHICS_SPRITEBASE_HPP_
#define INCLUDE_APG_GRAPHICS_SPRITEBASE_HPP_

#include <cstdint>

namespace APG {
class Texture;

class SpriteBase {
public:
	virtual ~SpriteBase() = default;
	virtual Texture * getTexture() const = 0;

	virtual uint32_t getWidth() const = 0;

	virtual uint32_t getHeight() const = 0;

	virtual float getU1() const = 0;
	virtual float getV1() const = 0;
	virtual float getU2() const = 0;
	virtual float getV2() const = 0;

	inline void setHash(uint64_t hash) {
		this->hash = hash;
	}

	inline uint64_t getHash() const {
		return hash;
	}

private:
	uint64_t hash = 0;
};

}

#endif /* INCLUDE_APG_SPRITEBASE_HPP_ */
