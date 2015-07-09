/*
 * Copyright (c) 2015 Ashley Davis (SgtCoDFish)
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdint>

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "APG/APGeasylogging.hpp"

#include "APG/SDLFontManager.hpp"
#include "APG/SpriteBase.hpp"
#include "APG/SXXDL.hpp"
#include "APG/Texture.hpp"
#include "APG/Sprite.hpp"

#include "APG/internal/Assert.hpp"

const int APG::SDLFontManager::MAX_OWNED_TEXTURES;
const int APG::SDLFontManager::MAX_OWNED_SPRITES;

APG::SDLFontManager::SDLFontManager() {

}

APG::FontManager::font_handle APG::SDLFontManager::loadFontFile(const std::string &filename, int pointSize) {
	auto logger = el::Loggers::getLogger("default");
	auto sdlFont = SXXDL::ttf::make_font_ptr(TTF_OpenFont(filename.c_str(), pointSize));

	if (!sdlFont) {
		logger->fatal("Couldn't load %v, error: %v", filename, TTF_GetError());
		return -1;
	} else {
		const auto handle = getNextFontHandle();

		loadedFonts.emplace(handle, StoredFont(handle, std::move(sdlFont)));

		logger->info("Loaded font \"%v\" with handle %v.", filename, handle);

		return handle;
	}
}

void APG::SDLFontManager::freeFont(font_handle &handle) {
	loadedFonts.erase(handle);
	freeFontHandle(handle);

	handle = -1;
}

void APG::SDLFontManager::setFontColor(const font_handle &handle, const glm::vec4 &color) {
	const auto &font = loadedFonts.find(handle);

	REQUIRE(font != loadedFonts.end(), "Can't change color of a font that doesn't exist!");

	font->second.color = glmToSDLColor(color);
}

glm::ivec2 APG::SDLFontManager::estimateSizeOf(const font_handle &fontHandle, const std::string &text) {
	const auto &font = loadedFonts.find(fontHandle);

	glm::ivec2 ret;

	if (TTF_SizeUTF8((*font).second.ptr.get(), text.c_str(), &ret.x, &ret.y)) {
		el::Loggers::getLogger("default")->error("Couldn't get size of text string, error: %v", TTF_GetError());
	}

	return std::move(ret);
}

APG::SpriteBase *APG::SDLFontManager::renderText(const font_handle &fontHandle, const std::string &text,
        FontRenderMethod method) {
	const auto logger = el::Loggers::getLogger("default");

	const auto &found = loadedFonts.find(fontHandle);

	REQUIRE(found != loadedFonts.end(), "Can't render text with a font that doesn't exist.");

	auto &font = (*found).second;

	SDL_Surface *tempSurface = nullptr;

	switch (method) {
	case FontRenderMethod::NICE: {
		// renders to a nice RGBA surface so we don't have to mess with it
		tempSurface = TTF_RenderUTF8_Blended(font.ptr.get(), text.c_str(), font.color);
		break;
	}

	case FontRenderMethod::FAST: {
		// renders to a palette so needs to be converted
		tempSurface = TTF_RenderUTF8_Solid(font.ptr.get(), text.c_str(), font.color);

		tempSurface = SDL_ConvertSurfaceFormat(tempSurface, SDL_PIXELFORMAT_RGBA8888, 0);
		break;
	}
	}

	if (tempSurface == nullptr) {
		logger->fatal("Couldn't render text string \"%v\" using font handle %v, error: %v", text, fontHandle,
		TTF_GetError());
		return nullptr;
	}

	const auto ownedTextureID = findAvailableOwnedTexture(tempSurface);

	if (ownedTextureID == -1) {
		logger->fatal("Ran out of space for textures for rendered text (max is %v textures). Time to refactor!",
		        APG::SDLFontManager::MAX_OWNED_TEXTURES);
		return nullptr;
	}

	const auto ownedSpriteID = findAvailableSpriteSlot();

	if (ownedSpriteID == -1) {
		logger->fatal("Ran out of space for sprites for rendered text (max is %v sprites). Time to refactor!",
		        APG::SDLFontManager::MAX_OWNED_SPRITES);
		return nullptr;
	}

	auto texture = std::make_unique<Texture>(tempSurface);
	ownedTextures[ownedTextureID] = std::move(texture);

	auto sprite = std::make_unique<Sprite>(ownedTextures[ownedTextureID].get());
	ownedSprites[ownedSpriteID] = std::move(sprite);

	return ownedSprites[ownedSpriteID].get();
}

SDL_Color APG::SDLFontManager::glmToSDLColor(const glm::vec4 &glmColor) {
	SDL_Color ret;

	ret.r = (uint8_t) (glmColor.r * 255.0f);
	ret.g = (uint8_t) (glmColor.g * 255.0f);
	ret.b = (uint8_t) (glmColor.b * 255.0f);
	ret.a = (uint8_t) (glmColor.a * 255.0f);

	return std::move(ret);
}

int APG::SDLFontManager::findAvailableOwnedTexture(SDL_Surface * const surface) const {
	for (int i = 0; i < MAX_OWNED_TEXTURES; ++i) {
		if (ownedTextures[i] == nullptr) {
			return i;
		}
	}

	return -1;
}

int APG::SDLFontManager::findAvailableSpriteSlot() const {
	for (int i = 0; i < MAX_OWNED_SPRITES; ++i) {
		if (ownedSprites[i] == nullptr) {
			return i;
		}
	}

	return -1;
}
