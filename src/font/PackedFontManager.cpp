#include "APG/font/PackedFontManager.hpp"
#include "APG/font/FontUtil.hpp"

#include "APG/internal/Assert.hpp"

namespace APG {


PackedFontManager::PackedFontManager(int packedTextureWidth, int packedTextureHeight) :
		packedTextureWidth{packedTextureWidth},
		packedTextureHeight{packedTextureHeight},
		packedTexture{nullptr},
		logger{spdlog::get("APG")} {

}

FontManager::font_handle PackedFontManager::loadFontFile(const std::string &filename, int pointSize) {
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

void PackedFontManager::freeFont(FontManager::font_handle &handle) {
	loadedFonts.erase(handle);
	freeFontHandle(handle);

	handle = -1;
}

void PackedFontManager::setFontColor(const FontManager::font_handle &handle, const glm::vec4 &color) {
	const auto &font = loadedFonts.find(handle);

	REQUIRE(font != loadedFonts.end(), "Can't change color of a font that doesn't exist!");

	font->second.color = glmToSDLColor(color);
}

glm::ivec2 PackedFontManager::estimateSizeOf(const FontManager::font_handle &fontHandle, const std::string &text) {
	const auto &font = loadedFonts.find(fontHandle);

	glm::ivec2 ret{};

	if (TTF_SizeUTF8((*font).second.ptr.get(), text.c_str(), &ret.x, &ret.y)) {
		logger->error("Couldn't get size of text string, error: {}", TTF_GetError());
	}

	return ret;
}

SpriteBase *PackedFontManager::renderText(const FontManager::font_handle &fontHandle, const std::string &text,
										  bool ignoreWhitespace, FontRenderMethod method) {
	const auto &found = loadedFonts.find(fontHandle);

	REQUIRE(found != loadedFonts.end(), "Can't render text with a font that doesn't exist.");

	auto &font = (*found).second;

	if (ignoreWhitespace) {
		return renderTextIgnoreWhitespace(font, text, method);
	} else {
		return renderTextWithWhitespace(font, text, method);
	}
}

SDL_Color PackedFontManager::glmToSDLColor(const glm::vec4 &glmColor) {
	SDL_Color ret;

	ret.r = (uint8_t) (glmColor.r * 255.0f);
	ret.g = (uint8_t) (glmColor.g * 255.0f);
	ret.b = (uint8_t) (glmColor.b * 255.0f);
	ret.a = (uint8_t) (glmColor.a * 255.0f);

	return ret;
}

SpriteBase *PackedFontManager::renderTextIgnoreWhitespace(const StoredSDLFont &font,
														  const std::string &text,
														  const FontRenderMethod method) {
	ensureTexture();
	auto tempSurface = SXXDL::make_surface_ptr(nullptr);

	switch (method) {
		case FontRenderMethod::NICE: {
			// renders to a nice RGBA surface so we don't have to mess with it
			tempSurface = SXXDL::make_surface_ptr(TTF_RenderUTF8_Blended(font.ptr.get(), text.c_str(), font.color));
			break;
		}

		case FontRenderMethod::FAST: {
			// renders to a palette so needs to be converted
			auto badFormatSurface = SXXDL::make_surface_ptr(
					TTF_RenderUTF8_Solid(font.ptr.get(), text.c_str(), font.color));

			tempSurface = SXXDL::make_surface_ptr(
					SDL_ConvertSurfaceFormat(badFormatSurface.get(), SDL_PIXELFORMAT_RGBA8888, 0));
			break;
		}
	}

	if (tempSurface == nullptr) {
		logger->critical("Couldn't render text string \"{}\" using font handle {}, error: {}", text, font.handle,
					  TTF_GetError());
		return nullptr;
	}

	const auto possibleRect = packedTexture->insertSurface(tempSurface.get());

	if (!possibleRect) {
		// failed to pack in some way
		logger->info("Failed to pack {}x{} text into existing packed texture", tempSurface->w,
					 tempSurface->h);
		return nullptr;
	}

	packedTexture->commitPack();

	const SDL_Rect rect = *possibleRect;

	ownedSprites.emplace_front(packedTexture.get(), rect.x, rect.y, rect.w, rect.h);

	auto spritePtr = &(ownedSprites.front());
	textToSprite[text] = spritePtr;

	return spritePtr;
}

SpriteBase *PackedFontManager::renderTextWithWhitespace(const StoredSDLFont &font, const std::string &text,
														const FontRenderMethod method) {
	ensureTexture();
	static const std::string wsDelimiter = "\n";

	std::vector<std::string> stringVector = util::splitString(text, wsDelimiter);
	std::vector<SXXDL::surface_ptr> surfaces;


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
					throw std::runtime_error("couldn't render string in PackedFontManager");
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
					throw std::runtime_error("couldn't render string in PackedFontManager");
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

	auto tempSurface = SXXDL::make_surface_ptr(
			SDL_CreateRGBSurface(0, maxWidth, totalHeight, surfaces.front()->format->BitsPerPixel,
								 surfaces.front()->format->Rmask, surfaces.front()->format->Gmask,
								 surfaces.front()->format->Bmask,
								 surfaces.front()->format->Amask));

	if (tempSurface == nullptr) {
		logger->critical("Couldn't create master surface for multi-line rendering with handle {}. Error: {}", font.handle,
					  SDL_GetError());
		throw std::runtime_error("couldn't render string in PackedFontManager");
		return nullptr;
	}

	auto hCounter = 0;
	for (const auto &surf : surfaces) {
		auto rect = SDL_Rect{0u, hCounter, 0u, 0u};
		if (SDL_BlitSurface(surf.get(), nullptr, tempSurface.get(), &rect) != 0) {
			logger->critical("Couldn't blit micro surface to multi-line master surface, error: {}", SDL_GetError());
			throw std::runtime_error("couldn't render string in PackedFontManager");
			return nullptr;
		}

		hCounter += surf->h;
	}

	const auto possibleRect = packedTexture->insertSurface(tempSurface.get());

	if (!possibleRect) {
		// failed to pack in some way
		logger->critical("Failed to pack {}x{} multi-line text into existing packed texture", tempSurface->w,
					 tempSurface->h);
		throw std::runtime_error("couldn't render string in PackedFontManager");
		return nullptr;
	}

	packedTexture->commitPack();

	const SDL_Rect rect = *possibleRect;

	ownedSprites.emplace_front(packedTexture.get(), rect.x, rect.y, rect.w, rect.h);

	auto spritePtr = &(ownedSprites.front());
	textToSprite[text] = spritePtr;

	return spritePtr;
}

void PackedFontManager::ensureTexture() {
	if(packedTexture == nullptr) {
		packedTexture = std::make_unique<PackedTexture>(packedTextureWidth, packedTextureHeight);
	}
}

}
