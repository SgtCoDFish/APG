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

#ifndef INCLUDE_APG_FONTMANAGER_HPP_
#define INCLUDE_APG_FONTMANAGER_HPP_

#include <cstdint>

#include <string>
#include <deque>
#include <type_traits>

#include <glm/glm.hpp>

#include "APG/APGCommon.hpp"

namespace APG {
class SpriteBase;

class FontManager {
public:
	using font_handle = int32_t;

	static_assert(std::is_copy_constructible<font_handle>(), "Font handle type must be copy constructible");

protected:
	std::deque<font_handle> availableFontHandles;
	void fillDefaultQueue(int initialFontHandleCount);

	font_handle getNextFontHandle();
	void freeFontHandle(font_handle handle);

public:
	explicit FontManager(int initialFontHandleCount = internal::DEFAULT_FONT_HANDLE_COUNT);
	virtual ~FontManager() = default;

	virtual font_handle loadFontFile(const std::string &filename, int pointSize) = 0;
	virtual void freeFont(font_handle &handle) = 0;

	virtual glm::ivec2 estimateSizeOf(const font_handle &fontHandle, const std::string &text) = 0;
	virtual SpriteBase *renderText(const font_handle &fontHandle, const std::string &text) = 0;
};

}

#endif /* INCLUDE_APG_FONTMANAGER_HPP_ */
