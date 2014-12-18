/*
 * SXXDL.hpp
 * Copyright (C) 2014 Ashley Davis (SgtCoDFish)
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
#ifndef SXXDL_HPP_
#define SXXDL_HPP_

#include <memory>

#include <SDL2/SDL.h>

namespace SXXDL {

using window_ptr = std::unique_ptr<SDL_Window, void(*)(SDL_Window *)>;
using surface_ptr = std::unique_ptr<SDL_Surface, void(*)(SDL_Surface *)>;
using texture_ptr = std::unique_ptr<SDL_Texture, void(*)(SDL_Texture *)>;
using renderer_ptr = std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer *)>;

window_ptr make_window_ptr(SDL_Window *window);
surface_ptr make_surface_ptr(SDL_Surface *surface);
texture_ptr make_texture_ptr(SDL_Texture *texture);
renderer_ptr make_renderer_ptr(SDL_Renderer *renderer);

}

#endif /* SXXDL_HPP_ */
