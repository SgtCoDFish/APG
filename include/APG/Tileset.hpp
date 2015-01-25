/*
 * Copyright (c) 2014, 2015 Ashley Davis (SgtCoDFish)
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

#ifndef APGTILESET_HPP_
#define APGTILESET_HPP_

#include <cstdint>

#include <string>
#include <memory>

#include "tmxparser/TmxMap.h"

#include "APG/Texture.hpp"

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
