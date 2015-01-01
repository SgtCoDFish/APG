/*
 * SXXDL.cpp
 * Copyright (C) 2014, 2015 Ashley Davis (SgtCoDFish)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "SXXDL.hpp"

SXXDL::window_ptr SXXDL::make_window_ptr(SDL_Window *window) {
	return window_ptr(window, SDL_DestroyWindow);
}

SXXDL::surface_ptr SXXDL::make_surface_ptr(SDL_Surface *surface) {
	return surface_ptr(surface, SDL_FreeSurface);
}

SXXDL::texture_ptr SXXDL::make_texture_ptr(SDL_Texture *texture) {
	return texture_ptr(texture, SDL_DestroyTexture);
}

SXXDL::renderer_ptr SXXDL::make_renderer_ptr(SDL_Renderer *renderer) {
	return renderer_ptr(renderer, SDL_DestroyRenderer);
}
