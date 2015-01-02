/*
 * Texture.hpp
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

#ifndef APGTILESET_HPP_
#define APGTILESET_HPP_

#include <cstdint>

#include <string>
#include <memory>

#include "TmxMap.h"

#include "Texture.hpp"

namespace Tmx {
class Map;
}

namespace APG {

class Tileset : public Texture {
private:
	int32_t tileWidth = 0;
	int32_t tileHeight = 0;

	int32_t widthInTiles = 0;
	int32_t heightInTiles = 0;

	void calculateWidthInTiles() {
		widthInTiles = getWidth() / tileWidth;
	}

	void calculateHeightInTiles() {
		heightInTiles = getHeight() / tileHeight;
	}

public:
	Tileset(const char * const fileName, Tmx::Map * const map) :
			Tileset(fileName, map->GetTileWidth(), map->GetTileHeight()) {
	}

	Tileset(const std::string &fileName, Tmx::Map * const map) :
			Tileset(fileName, map->GetTileWidth(), map->GetTileHeight()) {
	}

	Tileset(const char * const fileName, int32_t tileWidth, int32_t tileHeight) :
			Tileset(std::string(fileName), tileWidth, tileHeight) {
	}

	Tileset(const std::string &fileName, int32_t tileWidth, int32_t tileHeight) :
			Texture(fileName, true), tileWidth(tileWidth), tileHeight(tileHeight) {
		calculateWidthInTiles();
		calculateHeightInTiles();
	}

	int32_t getTileWidth() const {
		return tileWidth;
	}

	int32_t getTileHeight() const {
		return tileHeight;
	}

	int32_t getWidthInTiles() const {
		return widthInTiles;
	}

	int32_t getHeightInTiles() const {
		return heightInTiles;
	}
};

using tileset_ptr = std::unique_ptr<Tileset>;

}

#endif /* APGTILESET_HPP_ */
