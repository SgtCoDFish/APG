#ifndef INCLUDE_APG_SXXDL_INCLUDE_HPP_
#define INCLUDE_APG_SXXDL_INCLUDE_HPP_

#include <memory>

#include "APG/SDL.hpp"

namespace SXXDL {

using window_ptr = std::unique_ptr<SDL_Window, void(*)(SDL_Window *)>;
using surface_ptr = std::unique_ptr<SDL_Surface, void(*)(SDL_Surface *)>;
using sdl_texture_ptr = std::unique_ptr<SDL_Texture, void(*)(SDL_Texture *)>;
using renderer_ptr = std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer *)>;
using pixel_format_ptr = std::unique_ptr<SDL_PixelFormat, void(*)(SDL_PixelFormat *)>;

window_ptr make_window_ptr(SDL_Window *window);
surface_ptr make_surface_ptr(SDL_Surface *surface);
sdl_texture_ptr make_sdl_texture_ptr(SDL_Texture *texture);
renderer_ptr make_renderer_ptr(SDL_Renderer *renderer);
pixel_format_ptr make_pixel_format_ptr(SDL_PixelFormat *format);

namespace ttf {

using font_ptr = std::unique_ptr<TTF_Font, void(*)(TTF_Font *)>;
font_ptr make_font_ptr(TTF_Font *font);

}

namespace mixer {

using music_ptr = std::unique_ptr<Mix_Music, void(*)(Mix_Music *)>;
using sound_ptr = std::unique_ptr<Mix_Chunk, void (*)(Mix_Chunk *)>;

music_ptr make_music_ptr(Mix_Music *music);
sound_ptr make_sound_ptr(Mix_Chunk *chunk);

}

namespace net {

/**
 * This feels hideous because SDL does weird things with their typedefs and pointer types.
 */
using socket_set_ptr = std::unique_ptr<struct _SDLNet_SocketSet, void(*)(SDLNet_SocketSet)>;

socket_set_ptr make_socket_set_ptr(SDLNet_SocketSet socketSet);
}

}

#endif /* SXXDL_HPP_ */
