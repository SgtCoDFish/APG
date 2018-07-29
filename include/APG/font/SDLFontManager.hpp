#ifndef INCLUDE_APG_FONT_SDLFONTMANAGER_HPP_
#define INCLUDE_APG_FONT_SDLFONTMANAGER_HPP_

#ifndef APG_NO_SDL

#include <cstdint>

#include <unordered_map>
#include <array>

#include "spdlog/spdlog.h"

#include "APG/SDL.hpp"

#include "APG/SXXDL.hpp"
#include "APG/graphics/Texture.hpp"
#include "APG/graphics/Sprite.hpp"
#include "APG/graphics/PackedTexture.hpp"
#include "APG/font/FontManager.hpp"
#include "APG/font/StoredSDLFont.hpp"

namespace APG {
class SpriteBase;

class SDLFontManager : public FontManager {
public:
	explicit SDLFontManager();

	~SDLFontManager() override = default;

	font_handle loadFontFile(const std::string &filename, int pointSize) override;

	void freeFont(font_handle &handle) override;

	void setFontColor(const font_handle &handle, const glm::vec4 &color) override;

	glm::ivec2 estimateSizeOf(const font_handle &fontHandle, const std::string &text) override;

	SpriteBase *renderText(const font_handle &fontHandle, const std::string &text, bool ignoreWhitespace,
						   FontRenderMethod method) override;

private:
	static constexpr int MAX_OWNED_TEXTURES = 5;
	static constexpr int MAX_OWNED_SPRITES = 3;

	struct StoredSDLText {
		StoredSDLText(std::string text, int spriteID) :
				text{std::move(text)},
				spriteID{spriteID} {
		}

		const std::string text;
		const int spriteID;
	};

	std::unordered_map<font_handle, StoredSDLFont> loadedFonts;
	std::array<std::unique_ptr<Texture>, MAX_OWNED_TEXTURES> ownedTextures;
	std::array<std::unique_ptr<Sprite>, MAX_OWNED_SPRITES> ownedSprites;

	int findAvailableOwnedTexture(SDL_Surface *surface) const;

	int findAvailableSpriteSlot() const;

	SDL_Color glmToSDLColor(const glm::vec4 &glmColor);

	SpriteBase *renderTextIgnoreWhitespace(const StoredSDLFont &font, const std::string &text, FontRenderMethod method);

	SpriteBase *renderTextWithWhitespace(const StoredSDLFont &font, const std::string &text, FontRenderMethod method);

	std::shared_ptr<spdlog::logger> logger;
};

}

#endif

#endif
