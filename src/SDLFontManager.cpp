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

APG::SDLFontManager::SDLFontManager() {

}

APG::FontManager::font_handle APG::SDLFontManager::loadFontFile(const std::string &filename, int pointSize) {
	auto sdlFont = SXXDL::ttf::make_font_ptr(TTF_OpenFont(filename.c_str(), pointSize));

	if (!sdlFont) {
		el::Loggers::getLogger("default")->fatal("Couldn't load %v, error: %v", filename, TTF_GetError());
		return -1;
	} else {
		const auto handle = getNextFontHandle();

		loadedFonts.emplace(handle, std::move(sdlFont));

		return handle;
	}
}

void APG::SDLFontManager::freeFont(font_handle &handle) {
	loadedFonts.erase(handle);
	freeFontHandle(handle);

	handle = -1;
}

glm::ivec2 APG::SDLFontManager::estimateSizeOf(const font_handle &fontHandle, const std::string &text) {
	const auto &font = loadedFonts.find(fontHandle);

	glm::ivec2 ret;

	if (TTF_SizeUTF8((*font).second.get(), text.c_str(), &ret.x, &ret.y)) {
		el::Loggers::getLogger("default")->error("Couldn't get size of text string, error: %v", TTF_GetError());
	}

	return std::move(ret);
}

APG::SpriteBase *APG::SDLFontManager::renderText(const font_handle &fontHandle, const std::string &text) {

}

