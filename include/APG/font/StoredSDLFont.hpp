#ifndef APG_FONT_STOREDSDLFONT_HPP
#define APG_FONT_STOREDSDLFONT_HPP

#include <utility>

#include "APG/SDL.hpp"
#include "APG/SXXDL.hpp"

namespace APG {

struct StoredSDLFont {
	StoredSDLFont(FontManager::font_handle handle, SXXDL::ttf::font_ptr &&ptr) :
			handle{std::move(handle)},
			ptr{std::move(ptr)},
			color{0, 0, 0, 255} {
	}

	FontManager::font_handle handle{};
	SXXDL::ttf::font_ptr ptr;
	SDL_Color color;
};

}

#endif
