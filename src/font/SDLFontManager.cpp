#ifndef APG_NO_SDL

#include <cstdint>

#include <string>

#include "APG/SDL.hpp"

#include "APG/SXXDL.hpp"
#include "APG/font/SDLFontManager.hpp"
#include "APG/graphics/SpriteBase.hpp"
#include "APG/graphics/Texture.hpp"
#include "APG/graphics/Sprite.hpp"
#include "APG/internal/Assert.hpp"

namespace APG {

const int SDLFontManager::MAX_OWNED_TEXTURES;
const int SDLFontManager::MAX_OWNED_SPRITES;

SDLFontManager::SDLFontManager() : logger{spdlog::get("APG")} {
	logger->warn("SDLFontManager actively leaks when rendering, and should be avoided");
}

FontManager::font_handle SDLFontManager::loadFontFile(const std::string &filename, int pointSize) {
	auto sdlFont = SXXDL::ttf::make_font_ptr(TTF_OpenFont(filename.c_str(), pointSize));

	if (!sdlFont) {
		logger->critical("Couldn't load {}, error: {}", filename, TTF_GetError());
		return -1;
	} else {
		const auto handle = getNextFontHandle();

		loadedFonts.emplace(handle, StoredSDLFont(handle, std::move(sdlFont)));

		logger->info("Loaded font \"{}\" with handle {}.", filename, handle);

		return handle;
	}
}

void SDLFontManager::freeFont(font_handle &handle) {
	loadedFonts.erase(handle);
	freeFontHandle(handle);

	handle = -1;
}

void SDLFontManager::setFontColor(const font_handle &handle, const glm::vec4 &color) {
	const auto &font = loadedFonts.find(handle);

	REQUIRE(font != loadedFonts.end(), "Can't change color of a font that doesn't exist!");

	font->second.color = glmToSDLColor(color);
}

glm::ivec2 SDLFontManager::estimateSizeOf(const font_handle &fontHandle, const std::string &text) {
	const auto &font = loadedFonts.find(fontHandle);

	glm::ivec2 ret;

	if (TTF_SizeUTF8((*font).second.ptr.get(), text.c_str(), &ret.x, &ret.y)) {
		logger->error("Couldn't get size of text string, error: {}", TTF_GetError());
	}

	return ret;
}

SpriteBase *SDLFontManager::renderText(const font_handle &fontHandle, const std::string &text,
									   bool ignoreWhitespace, const FontRenderMethod method) {
	const auto &found = loadedFonts.find(fontHandle);

	REQUIRE(found != loadedFonts.end(), "Can't render text with a font that doesn't exist.");

	auto &font = (*found).second;

	if (ignoreWhitespace) {
		return renderTextIgnoreWhitespace(font, text, method);
	} else {
		return renderTextWithWhitespace(font, text, method);
	}
}

SDL_Color SDLFontManager::glmToSDLColor(const glm::vec4 &glmColor) {
	SDL_Color ret;

	ret.r = (uint8_t) (glmColor.r * 255.0f);
	ret.g = (uint8_t) (glmColor.g * 255.0f);
	ret.b = (uint8_t) (glmColor.b * 255.0f);
	ret.a = (uint8_t) (glmColor.a * 255.0f);

	return ret;
}

int SDLFontManager::findAvailableOwnedTexture(SDL_Surface *surface) const {
	for (int i = 0; i < MAX_OWNED_TEXTURES; ++i) {
		if (ownedTextures[i] == nullptr) {
			return i;
		}
	}

	return -1;
}

int SDLFontManager::findAvailableSpriteSlot() const {
	for (int i = 0; i < MAX_OWNED_SPRITES; ++i) {
		if (ownedSprites[i] == nullptr) {
			return i;
		}
	}

	return -1;
}

SpriteBase *SDLFontManager::renderTextIgnoreWhitespace(const StoredSDLFont &font,
													   const std::string &text,
													   const FontRenderMethod method) {
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
		logger->critical("Couldn't render text string \"{}\" using font handle {}, error: {}", text, font.handle,
					  TTF_GetError());
		return nullptr;
	}

	const auto ownedTextureID = findAvailableOwnedTexture(tempSurface);

	if (ownedTextureID == -1) {
		logger->critical("Ran out of space for textures for rendered text (max is {} textures). Time to refactor!",
					  APG::SDLFontManager::MAX_OWNED_TEXTURES);
		return nullptr;
	}

	const auto ownedSpriteID = findAvailableSpriteSlot();

	if (ownedSpriteID == -1) {
		logger->critical("Ran out of space for sprites for rendered text (max is {} sprites). Time to refactor!",
					  APG::SDLFontManager::MAX_OWNED_SPRITES);
		return nullptr;
	}

	auto texture = std::make_unique<Texture>(tempSurface);
	ownedTextures[ownedTextureID] = std::move(texture);

	auto sprite = std::make_unique<Sprite>(ownedTextures[ownedTextureID].get());
	ownedSprites[ownedSpriteID] = std::move(sprite);

	return ownedSprites[ownedSpriteID].get();
}

SpriteBase *SDLFontManager::renderTextWithWhitespace(const StoredSDLFont &font, const std::string &text,
													 const FontRenderMethod method) {
	static const std::string wsDelimiter = "\n";

	std::vector<std::string> stringVector;
	std::vector<SXXDL::surface_ptr> surfaces;

	std::string::size_type begin = 0U;
	auto end = text.find(wsDelimiter);

	while (end != std::string::npos) {
		stringVector.emplace_back(text.substr(begin, end - begin));
		begin = end + wsDelimiter.length();
		end = text.find(wsDelimiter, begin);
	}

	if (stringVector.empty()) {
		return renderTextIgnoreWhitespace(font, text, method);
	}

	switch (method) {
		case FontRenderMethod::NICE: {
			for (const auto &s : stringVector) {
				// renders to a nice RGBA surface so we don't have to mess with it
				auto surface = TTF_RenderUTF8_Blended(font.ptr.get(), s.c_str(), font.color);

				if (surface == nullptr) {
					logger->critical("Couldn't render text string \"{}\" ({} of {}) using font handle {}. Error: {}", s,
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
					logger->critical("Couldn't render text string \"{}\" ({} of {}) using font handle {}. Error: {}", s,
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
													surfaces.front()->format->Rmask, surfaces.front()->format->Gmask,
													surfaces.front()->format->Bmask,
													surfaces.front()->format->Amask);

	if (tempSurface == nullptr) {
		logger->critical("Couldn't create master surface for multi-line rendering with handle {}. Error: {}", font.handle,
					  SDL_GetError());
		return nullptr;
	}

	auto hCounter = 0;
	for (const auto &surf : surfaces) {
		auto rect = SDL_Rect{0u, hCounter, 0u, 0u};
		if (SDL_BlitSurface(surf.get(), nullptr, tempSurface, &rect) != 0) {
			logger->critical("Couldn't blit micro surface to multi-line master surface, error: {}", SDL_GetError());
			SDL_FreeSurface(tempSurface);
			return nullptr;
		}

		hCounter += surf->h;
	}

	const auto ownedTextureID = findAvailableOwnedTexture(tempSurface);

	if (ownedTextureID == -1) {
		logger->critical("Ran out of space for textures for rendered text (max is {} textures). Time to refactor!",
					  APG::SDLFontManager::MAX_OWNED_TEXTURES);
		return nullptr;
	}

	const auto ownedSpriteID = findAvailableSpriteSlot();

	if (ownedSpriteID == -1) {
		logger->critical("Ran out of space for sprites for rendered text (max is {} sprites). Time to refactor!",
					  APG::SDLFontManager::MAX_OWNED_SPRITES);
		return nullptr;
	}

	auto texture = std::make_unique<Texture>(tempSurface);
	ownedTextures[ownedTextureID] = std::move(texture);

	auto sprite = std::make_unique<Sprite>(ownedTextures[ownedTextureID]);
	ownedSprites[ownedSpriteID] = std::move(sprite);

	return ownedSprites[ownedSpriteID].get();
}

}

#endif
