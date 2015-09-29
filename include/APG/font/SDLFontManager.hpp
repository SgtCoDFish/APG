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

#ifndef INCLUDE_APG_FONT_SDLFONTMANAGER_HPP_
#define INCLUDE_APG_FONT_SDLFONTMANAGER_HPP_

#include <cstdint>

#include <unordered_map>
#include <array>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "APG/SXXDL.hpp"
#include "APG/graphics/Texture.hpp"
#include "APG/graphics/Sprite.hpp"
#include "APG/font/FontManager.hpp"

namespace APG {
class SpriteBase;

class SDLFontManager : public FontManager {
public:
	explicit SDLFontManager();
	virtual ~SDLFontManager() = default;

	virtual font_handle loadFontFile(const std::string &filename, int pointSize) override;
	virtual void freeFont(font_handle &handle) override;

	virtual void setFontColor(const font_handle &handle, const glm::vec4 &color) override;

	virtual glm::ivec2 estimateSizeOf(const font_handle &fontHandle, const std::string &text) override;
	virtual SpriteBase *renderText(const font_handle &fontHandle, const std::string &text, bool ignoreWhitespace = true,
	        const FontRenderMethod method = FontRenderMethod::FAST) override;

private:
	static constexpr int MAX_OWNED_TEXTURES = 5;
	static constexpr int MAX_OWNED_SPRITES = 3;

	struct StoredFont {
		StoredFont(const font_handle &handle, SXXDL::ttf::font_ptr &&ptr) :
				        handle { handle },
				        ptr { std::move(ptr) },
				        color { 0, 0, 0, 255 } {
		}

		font_handle handle;
		SXXDL::ttf::font_ptr ptr;
		SDL_Color color;
	};

	struct StoredSDLText {
		StoredSDLText(const std::string &text, const int spriteID) :
				        text { text },
				        spriteID { spriteID } {
		}

		const std::string text;
		const int spriteID;
	};

	std::unordered_map<font_handle, StoredFont> loadedFonts;
	std::array<std::unique_ptr<Texture>, MAX_OWNED_TEXTURES> ownedTextures;
	std::array<std::unique_ptr<Sprite>, MAX_OWNED_SPRITES> ownedSprites;

	int findAvailableOwnedTexture(SDL_Surface *surface) const;
	int findAvailableSpriteSlot() const;

	SDL_Color glmToSDLColor(const glm::vec4 &glmColor);

	SpriteBase * renderTextIgnoreWhitespace(const StoredFont &font, const std::string &text, const FontRenderMethod method, el::Logger * const logger);
	SpriteBase * renderTextWithWhitespace(const StoredFont &font, const std::string &text, const FontRenderMethod method, el::Logger * const logger);
};

}

#endif /* INCLUDE_APG_SDLFONTMANAGER_HPP_ */
