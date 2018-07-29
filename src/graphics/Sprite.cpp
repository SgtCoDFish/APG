#ifndef APG_NO_GL

#include <cstdint>

#include <memory>
#include <string>
#include <sstream>

#include "APG/graphics/Sprite.hpp"
#include "APG/graphics/Texture.hpp"
#include "APG/internal/Assert.hpp"

namespace APG {

Sprite::Sprite(Texture *texture) :
		Sprite(texture, 0, 0, texture->getWidth(), texture->getHeight()) {

}

Sprite::Sprite(Texture *texture, int32_t texX, int32_t texY, int32_t width, int32_t height) :
		texture(texture),
		texX(texX),
		texY(texY),
		width(width),
		height(height),
		logger{spdlog::get("APG")} {
	if (texture == nullptr) {
		logger->critical("Can't pass a null texture to create a sprite.");
		return;
	}

	if (texX + width > texture->getWidth() || texY + height > texture->getHeight()) {
		logger->error(
				"Trying to create sprite with (texX, texY, w, h) = ({}, {}, {}, {}) would exceed texture size (w, h) = ({}, {})",
				texX, texY, width, height, texture->getWidth(), texture->getHeight()
		);
	}
	logger->trace("Creating sprite (x, y, w, h) = ({}, {}, {}, {})", texX, texY, width, height);
	calculateUV();
}

void Sprite::calculateUV() {
	u1 = texture->getInvWidth() * texX;
	v1 = texture->getInvHeight() * texY;

	u2 = texture->getInvWidth() * (texX + width);
	v2 = texture->getInvHeight() * (texY + height);
}

}

#endif
