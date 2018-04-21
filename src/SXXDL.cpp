#ifndef APG_NO_SDL

#include "APG/SDL.hpp"
#include "APG/SXXDL.hpp"

SXXDL::window_ptr SXXDL::make_window_ptr(SDL_Window *window) {
	return window_ptr(window, SDL_DestroyWindow);
}

SXXDL::surface_ptr SXXDL::make_surface_ptr(SDL_Surface *surface) {
	return surface_ptr(surface, SDL_FreeSurface);
}

SXXDL::sdl_texture_ptr SXXDL::make_sdl_texture_ptr(SDL_Texture *texture) {
	return sdl_texture_ptr(texture, SDL_DestroyTexture);
}

SXXDL::renderer_ptr SXXDL::make_renderer_ptr(SDL_Renderer *renderer) {
	return renderer_ptr(renderer, SDL_DestroyRenderer);
}

SXXDL::pixel_format_ptr SXXDL::make_pixel_format_ptr(SDL_PixelFormat *format) {
	return pixel_format_ptr(format, SDL_FreeFormat);
}

SXXDL::ttf::font_ptr SXXDL::ttf::make_font_ptr(TTF_Font *font) {
	return font_ptr(font, TTF_CloseFont);
}

SXXDL::mixer::sound_ptr SXXDL::mixer::make_sound_ptr(Mix_Chunk *chunk) {
	return sound_ptr(chunk, Mix_FreeChunk);
}

SXXDL::mixer::music_ptr SXXDL::mixer::make_music_ptr(Mix_Music *music) {
	return music_ptr(music, Mix_FreeMusic);
}

SXXDL::net::socket_set_ptr SXXDL::net::make_socket_set_ptr(SDLNet_SocketSet socketSet) {
	return socket_set_ptr(socketSet, SDLNet_FreeSocketSet);
}

#endif
