#ifndef INCLUDE_APG_FONT_FONTMANAGER_HPP_
#define INCLUDE_APG_FONT_FONTMANAGER_HPP_

#include <cstdint>

#include <string>
#include <deque>
#include <type_traits>

#include <glm/glm.hpp>

#include "APG/core/APGCommon.hpp"

namespace APG {
class SpriteBase;

enum class FontRenderMethod {
	FAST, NICE
};

class FontManager {
public:
	using font_handle = int32_t;

	static_assert(std::is_copy_constructible<font_handle>(), "Font handle type must be copy constructible");

	explicit FontManager(int initialFontHandleCount = internal::DEFAULT_FONT_HANDLE_COUNT);

	virtual ~FontManager() = default;

	virtual font_handle loadFontFile(const std::string &filename, int pointSize) = 0;

	virtual void freeFont(font_handle &handle) = 0;

	virtual void setFontColor(const font_handle &handle, const glm::vec4 &color) = 0;

	virtual glm::ivec2 estimateSizeOf(const font_handle &fontHandle, const std::string &text) = 0;

	virtual SpriteBase *renderText(const font_handle &fontHandle, const std::string &text, bool ignoreWhitespace,
								   FontRenderMethod method) = 0;

protected:
	std::deque<font_handle> availableFontHandles;

	void fillDefaultQueue(int initialFontHandleCount);

	font_handle getNextFontHandle();

	void freeFontHandle(font_handle handle);
};

}

#endif
