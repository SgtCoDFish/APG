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

#ifndef GENTMXRENDERER_HPP_
#define GENTMXRENDERER_HPP_

#include <vector>
#include <unordered_map>

#include "APG/APGCommon.hpp"
#include "APG/ErrorBase.hpp"
#include "APG/Tileset.hpp"
#include "APG/SXXDL.hpp"
#include "APG/AnimatedSprite.hpp"

#include <glm/vec2.hpp>

namespace Tmx {
class Map;
class Tile;
}

namespace APG {

/**
 * Abstracts a renderer for a TMX file that can be loaded using the tmxparser library.
 *
 * Note that sprites from tilesets are stored in a map which hashes based on a uint64_t.
 * Normally this is completely transparent and you don't need to worry about it at all,
 * but it assumes that there are no more than 1,000,000 sprites in a single OpenGL texture
 * unit. This would generally be far more than you'd ever see, but things will break if
 * this happens so you should be aware.
 */
class TmxRenderer: public ErrorBase {
protected:
	static const uint64_t MAX_SPRITES_PER_UNIT = 1000000;

	Tmx::Map *map = nullptr;
	std::vector<tileset_ptr> tilesets;
	std::unordered_map<uint64_t, SpriteBase *> sprites;

	std::vector<Sprite> loadedSprites;
	std::vector<AnimatedSprite> loadedAnimatedSprites;

	glm::vec2 position { 0.0f, 0.0f };

	void loadTilesets();
	uint64_t calculateTileHash(const Tileset *tileset, Tmx::Tile * const tile) const;
	uint64_t calculateTileHash(const Tileset *tileset, int tileID) const;

public:
	explicit TmxRenderer(Tmx::Map *map);

	virtual ~TmxRenderer() {
	}

	virtual void renderLayer(Tmx::Layer * const layer) = 0;
	void renderAll();

	const Tmx::Map *getMap() {
		return map;
	}

	const glm::vec2 &getPosition() const {
		return position;
	}

	void setPosition(glm::vec2 &position) {
		this->position = position;
	}

	Tileset * getTilesetByID(int32_t id) const {
		try {
			if(tilesets.at(id) == nullptr) {
				return nullptr;
			}

			return tilesets.at(id).get();
		} catch (std::out_of_range &oor) {
			return nullptr;
		}
	}

	TmxRenderer(TmxRenderer &other) = delete;
	TmxRenderer(const TmxRenderer &other) = delete;
	TmxRenderer &operator=(TmxRenderer &other) = delete;
	TmxRenderer &operator=(const TmxRenderer &other) = delete;
};

}

#endif /* GENTMXRENDERER_HPP_ */
