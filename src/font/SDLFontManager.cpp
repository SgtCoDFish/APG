/*
 * Copyright (c) 2015 See AUTHORS file.
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

#ifndef APG_NO_SDL

#include <cstdint>

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "APG/SXXDL.hpp"
#include "APG/core/APGeasylogging.hpp"
#include "APG/font/SDLFontManager.hpp"
#include "APG/graphics/SpriteBase.hpp"
#include "APG/graphics/Texture.hpp"
#include "APG/graphics/Sprite.hpp"
#include "APG/internal/Assert.hpp"

const int APG::SDLFontManager::MAX_OWNED_TEXTURES;
const int APG::SDLFontManager::MAX_OWNED_SPRITES;

APG::SDLFontManager::SDLFontManager() {

}

APG::FontManager::font_handle APG::SDLFontManager::loadFontFile(const std::string &filename, int pointSize) {
	auto logger = el::Loggers::getLogger("APG");
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
		el::Loggers::getLogger("APG")->error("Couldn't get size of text string, error: %v", TTF_GetError());
	}

	return ret;
}

APG::SpriteBase *APG::SDLFontManager::renderText(const font_handle &fontHandle, const std::string &text,
bool ignoreWhitespace, const FontRenderMethod method) {
	const auto logger = el::Loggers::getLogger("APG");

	const auto &found = loadedFonts.find(fontHandle);

	REQUIRE(found != loadedFonts.end(), "Can't render text with a font that doesn't exist.");

	auto &font = (*found).second;

	if (ignoreWhitespace) {
		return renderTextIgnoreWhitespace(font, text, method, logger);
	} else {
		return renderTextWithWhitespace(font, text, method, logger);
	}
}

SDL_Color APG::SDLFontManager::glmToSDLColor(const glm::vec4 &glmColor) {
	SDL_Color ret;

	ret.r = (uint8_t) (glmColor.r * 255.0f);
	ret.g = (uint8_t) (glmColor.g * 255.0f);
	ret.b = (uint8_t) (glmColor.b * 255.0f);
	ret.a = (uint8_t) (glmColor.a * 255.0f);

	return ret;
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

APG::SpriteBase * APG::SDLFontManager::renderTextIgnoreWhitespace(const APG::SDLFontManager::StoredFont &font,
        const std::string &text, const APG::FontRenderMethod method, el::Logger * const logger) {
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
		logger->fatal("Couldn't render text string \"%v\" using font handle %v, error: %v", text, font.handle,
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

APG::SpriteBase * APG::SDLFontManager::renderTextWithWhitespace(const StoredFont &font, const std::string &text,
        const FontRenderMethod method, el::Logger * const logger) {
	static const std::string wsDelim = "\n";

	std::vector<std::string> stringVector;
	std::vector<SXXDL::surface_ptr> surfaces;

	std::string::size_type begin = 0U;
	auto end = text.find(wsDelim);

	while (end != std::string::npos) {
		stringVector.emplace_back(text.substr(begin, end-begin));
		begin = end + wsDelim.length();
		end = text.find(wsDelim, begin);
	}

	if(stringVector.empty()) {
		return renderTextIgnoreWhitespace(font, text, method, logger);
	}

	switch (method) {
	case FontRenderMethod::NICE: {
		for (const auto &s : stringVector) {
			// renders to a nice RGBA surface so we don't have to mess with it
			auto surface = TTF_RenderUTF8_Blended(font.ptr.get(), s.c_str(), font.color);

			if (surface == nullptr) {
				logger->fatal("Couldn't render text string \"%v\" (%v of %v) using font handle %v. Error: %v", s,
				        surfaces.size() + 1, stringVector.size(), font.handle, TTF_GetError());
				return nullptr;
			}

			surfaces.emplace_back(SXXDL::make_surface_ptr(surface));
		}
		break;
	}

	case FontRenderMethod::FAST: {
		for (const auto &s : stringVector) {
			// renders to a palette so needs to be converted
			auto surface = TTF_RenderUTF8_Solid(font.ptr.get(), s.c_str(), font.color);

			if (surface == nullptr) {
				logger->fatal("Couldn't render text string \"%v\" (%v of %v) using font handle %v. Error: %v", s,
				        surfaces.size() + 1, stringVector.size(), font.handle, TTF_GetError());
				return nullptr;
			}

			surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);

			surfaces.emplace_back(SXXDL::make_surface_ptr(surface));
		}

		break;
	}
	}

	auto maxWidth = 0, totalHeight = 0;

	for (const auto &surf : surfaces) {
		if (surf->w > maxWidth) {
			maxWidth = surf->w;
		}

		totalHeight += surf->h;
	}

	SDL_Surface *tempSurface = SDL_CreateRGBSurface(0, maxWidth, totalHeight, surfaces.front()->format->BitsPerPixel,
	        surfaces.front()->format->Rmask, surfaces.front()->format->Gmask, surfaces.front()->format->Bmask,
	        surfaces.front()->format->Amask);

	if (tempSurface == nullptr) {
		logger->fatal("Couldn't create master surface for multiline rendering with handle %v. Error: %v", font.handle,
		        SDL_GetError());
		return nullptr;
	}

	auto hCounter = 0;
	for (const auto &surf : surfaces) {
		auto rect = SDL_Rect { 0u, hCounter, 0u, 0u };
		if(SDL_BlitSurface(surf.get(), nullptr, tempSurface, &rect) != 0) {
			logger->fatal("Couldn't blit micro surface to multiline master surface, error: %v", SDL_GetError());
			return nullptr;
		}

		hCounter += surf->h;
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

	auto sprite = std::make_unique<Sprite>(ownedTextures[ownedTextureID]);
	ownedSprites[ownedSpriteID] = std::move(sprite);

	return ownedSprites[ownedSpriteID].get();
}

#endif
