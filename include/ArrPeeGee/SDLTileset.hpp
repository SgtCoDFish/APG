/*
 * SDLTileset.hpp
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

#ifndef SDLTILESET_HPP_
#define SDLTILESET_HPP_

#include <memory>
#include <utility>

#include "SXXDL.hpp"

namespace APG {


/**
 * Wrapper around an SDL texture along with a width and height.
 */
class SDLTileset {
public:
	/** Width in pixels. */
	const int w;

	/** Height in pixels. */
	const int h;

	const int width_in_tiles;
	const int height_in_tiles;

	const SXXDL::texture_ptr texture;

	SDLTileset(map_ptr &map, int w, int h, SXXDL::texture_ptr &texture) :
			w { w }, h { h }, width_in_tiles{w / map->GetTileWidth()}, height_in_tiles{h / map->GetTileHeight()}, texture { std::move(texture) } {
	}

	SDLTileset(SDLTileset &other) = delete;
	SDLTileset(const SDLTileset &other) = delete;
};

using tileset_ptr = std::unique_ptr<SDLTileset>;

}

#endif /* SDLTILESET_HPP_ */
