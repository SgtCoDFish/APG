#include "APG/core/APGCommon.hpp"
#include "APG/font/FontManager.hpp"

namespace APG {

FontManager::FontManager(int initialFontHandleCount) {
	fillDefaultQueue(initialFontHandleCount);
}

FontManager::font_handle APG::FontManager::getNextFontHandle() {
	const auto retVal = availableFontHandles.front();
	availableFontHandles.pop_front();
	return retVal;
}

void FontManager::freeFontHandle(font_handle handle) {
	availableFontHandles.push_front(handle);
}

void FontManager::fillDefaultQueue(int initialFontHandleCount) {
	for (font_handle i = 0; i < initialFontHandleCount; ++i) {
		availableFontHandles.push_back(i);
	}
}

}
