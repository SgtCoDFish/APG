#include "APG/internal/Assert.hpp"
#include "APG/graphics/PackedTexture.hpp"

namespace APG {

PackedTexture::PackedTexture(int32_t width, int32_t height) :
		Texture(/* TODO: create sdl surface*/static_cast<SDL_Surface *>(nullptr)) {

}

void PackedTexture::beginPack() {
	REQUIRE(!packing, "cannot begin packing when already packing");
	packing = true;
}

void PackedTexture::endPack() {
	REQUIRE(packing, "cannot end packing when not packing");
	// TODO: blitting + texture uploading goes to here.
	packing = false;
}

}