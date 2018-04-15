#ifndef APG_NO_GL

#include <cstdint>

#include <memory>
#include <string>
#include <sstream>

#include "APG/graphics/Sprite.hpp"
#include "APG/graphics/Texture.hpp"
#include "APG/internal/Assert.hpp"

APG::Sprite::Sprite(Texture * texture) :
		        Sprite(texture, 0, 0, texture->getWidth(), texture->getHeight()) {

}

APG::Sprite::Sprite(Texture * texture, uint32_t texX, uint32_t texY, uint32_t width, uint32_t height) :
		        texture(texture),
		        texX(texX),
		        texY(texY),
		        width(width),
		        height(height) {
	REQUIRE(texture != nullptr, "Can't pass a null texture to Sprite::Sprite");

	REQUIRE(texX + width <= texture->getWidth() && texY + height <= texture->getHeight(),
	        "Invalid texture sizes when creating sprite.");

	calculateUV();
}

void APG::Sprite::calculateUV() {
	u1 = texture->getInvWidth() * texX;
	v1 = texture->getInvHeight() * texY;

	u2 = texture->getInvWidth() * (texX + width);
	v2 = texture->getInvHeight() * (texY + height);
}

#endif
