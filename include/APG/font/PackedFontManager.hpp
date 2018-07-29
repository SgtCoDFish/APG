#ifndef APG_FONT_PACKEDFONTMANAGER_HPP
#define APG_FONT_PACKEDFONTMANAGER_HPP

#include <memory>
#include <forward_list>
#include <unordered_map>

#include <glm/vec4.hpp>

#include "spdlog/spdlog.h"

#include "APG/font/FontManager.hpp"
#include "APG/font/StoredSDLFont.hpp"
#include "APG/graphics/PackedTexture.hpp"

namespace APG {

class PackedFontManager final : public FontManager {
public:
	explicit PackedFontManager(int packedTextureWidth, int packedTextureHeight);

	~PackedFontManager() override = default;

	font_handle loadFontFile(const std::string &filename, int pointSize) override;

	void freeFont(font_handle &handle) override;

	void setFontColor(const font_handle &handle, const glm::vec4 &color) override;

	glm::ivec2 estimateSizeOf(const font_handle &fontHandle, const std::string &text) override;

	SpriteBase *renderText(const font_handle &fontHandle, const std::string &text, bool ignoreWhitespace,
						   FontRenderMethod method) override;

private:
	int packedTextureWidth;
	int packedTextureHeight;
	std::unique_ptr<PackedTexture> packedTexture;

	void ensureTexture();

	std::unordered_map<font_handle, StoredSDLFont> loadedFonts;

	std::forward_list<Sprite> ownedSprites;
	std::unordered_map<std::string, SpriteBase *> textToSprite;

	SDL_Color glmToSDLColor(const glm::vec4 &glmColor);

	SpriteBase *renderTextIgnoreWhitespace(const StoredSDLFont &font, const std::string &text, FontRenderMethod method);

	SpriteBase *renderTextWithWhitespace(const StoredSDLFont &font, const std::string &text, const FontRenderMethod method);

	std::shared_ptr<spdlog::logger> logger;
};

}

#endif
