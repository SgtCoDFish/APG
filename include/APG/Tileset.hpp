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
#include "tmxparser/TmxTileset.h"
#include "tmxparser/TmxImage.h"

#include "APG/APGeasylogging.hpp"

#include "APG/Texture.hpp"

namespace APG {

/**
 * A tileset from which tiles can be picked. Note that if there is a non-zero spacing between tiles, this is expected to be uniformly applied; that is, it is expected that:
 * (imageWidth) % (tileWidth + spacing) == 0.
 */
class Tileset: public Texture {
private:
	int32_t tileWidth = 0;
	int32_t tileHeight = 0;

	int32_t widthInTiles = 0;
	int32_t heightInTiles = 0;

	int32_t spacing = 0;

	void calculateWidthInTiles() {
		const auto gap = tileWidth + spacing;

		if (getWidth() % gap != 0) {
			el::Loggers::getLogger("APG")->warn(
			        "Tileset %v may have an inconsistent spacing in the x direction; this could cause issues.", this->getFileName());
		}

		widthInTiles = getWidth() / gap;
	}

	void calculateHeightInTiles() {
		const auto gap = tileHeight + spacing;

		if(getHeight() % gap != 0) {
			el::Loggers::getLogger("APG")->warn(
			        "Tileset %v may have an inconsistent spacing in the y direction; this could cause issues.", this->getFileName());
		}

		heightInTiles = getHeight() / gap;
	}

public:
	/**
	 * Create a new tileset based on a Tmx Map. Normally, you'll want to use Tileset(std::string, Tmx::Tileset *) instead.
	 *
	 * Assumes that tiles are tightly packed; that is, assumes there is no spacing between tiles.
	 * @param fileName The location of the tileset to load.
	 * @param map The map whose tile widths we'll be using for this tileset.
	 */
	explicit Tileset(const std::string &fileName, Tmx::Map * const map) :
			Tileset(fileName, map->GetTileWidth(), map->GetTileHeight()) {
	}

	/**
	 * Creates a new tileset based on a Tmx::Tileset. Correctly handly spacing of tiles.
	 *
	 * @param fileName The location of the tileset to load.
	 * @param tileset The tmx tileset describing the tileset. This information is parsed, and spacing information is used appropriately.
	 */
	explicit Tileset(const std::string &fileName, Tmx::Tileset * const tileset) :
			Tileset(fileName, tileset->GetTileWidth(), tileset->GetTileHeight(), tileset->GetSpacing()) {
	}

	explicit Tileset(const std::string &fileName, int32_t tileWidth, int32_t tileHeight, int32_t spacing = 0) :
			Texture { fileName, true }, tileWidth { tileWidth }, tileHeight { tileHeight }, spacing { spacing } {
		calculateWidthInTiles();
		calculateHeightInTiles();
	}

	virtual ~Tileset() = default;

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

	int32_t getSpacing() const {
		return spacing;
	}
};

}

#endif /* APGTILESET_HPP_ */
