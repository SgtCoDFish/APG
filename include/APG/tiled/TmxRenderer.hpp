/*
 * Copyright (c) 2014, 2015 See AUTHORS file.
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

#ifndef INCLUDE_APG_TILED_TMXRENDERER_HPP_
#define INCLUDE_APG_TILED_TMXRENDERER_HPP_

#include <vector>
#include <unordered_map>

#include <glm/vec2.hpp>

#include "APG/SXXDL.hpp"
#include "APG/core/APGCommon.hpp"
#include "APG/graphics/Tileset.hpp"
#include "APG/graphics/AnimatedSprite.hpp"

namespace Tmx {
class Map;
class Tile;
}

namespace APG {

class TiledObject {
public:
	explicit TiledObject(const glm::vec2 &pos, SpriteBase * const sprite) :
			        position { pos },
			        sprite { sprite } {
	}
	explicit TiledObject(float x, float y, SpriteBase * const sprite) :
			        position { x, y },
			        sprite { sprite } {
	}

	glm::vec2 position;
	SpriteBase * sprite;
};

/**
 * Abstracts a renderer for a TMX file that can be loaded using the tmxparser library.
 *
 * Note that sprites from tilesets are stored in a hashmap which hashes based on a uint64_t.
 * Normally this is completely transparent and you don't need to worry about it at all,
 * but it assumes that there are no more than 1,000,000 sprites in a single OpenGL texture
 * unit. This would generally be far more than you'd ever see, but things will break if
 * this happens so you should be aware.
 */
class TmxRenderer {
public:
	// TAKES OWNERSHIP
	explicit TmxRenderer(std::unique_ptr<Tmx::Map> &&map);
	explicit TmxRenderer(Tmx::Map * const map);
	explicit TmxRenderer(const std::string &fileName);

	virtual ~TmxRenderer() = default;

	virtual void renderLayer(const Tmx::TileLayer * const layer) = 0;
	virtual void renderObjectGroup(const std::vector<TiledObject> &objects) = 0;

	/**
	 * Renders all the layers and calls update for you.
	 */
	void renderAll(float deltaTime);
	void update(float deltaTime);

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
		if (tilesets[id] == nullptr) {
			return nullptr;
		}

		return tilesets[id].get();
	}

	TmxRenderer(TmxRenderer &other) = delete;
	TmxRenderer(const TmxRenderer &other) = delete;
	TmxRenderer &operator=(TmxRenderer &other) = delete;
	TmxRenderer &operator=(const TmxRenderer &other) = delete;

protected:
	static const uint64_t MAX_SPRITES_PER_UNIT = 1000000;
	static std::unordered_map<std::string, std::shared_ptr<Tileset>> tmxTilesets;
	static void initialiseStaticTilesets();

	Tmx::Map *map = nullptr;
	std::vector<std::shared_ptr<Tileset>> tilesets;
	std::unordered_map<uint64_t, SpriteBase *> sprites;

	std::vector<Sprite> loadedSprites;
	std::vector<AnimatedSprite> loadedAnimatedSprites;

	std::unordered_map<std::string, std::vector<TiledObject>> objectGroups;

	glm::vec2 position { 0.0f, 0.0f };

	void loadTilesets();
	void loadObjects();

	uint64_t calculateTileGID(Tmx::Tileset *tileset, int tileID);
	uint64_t calculateTileGID(Tmx::Tileset *tileset, Tmx::Tile *tile);

private:
	std::unique_ptr<Tmx::Map> ownedMap;

	void init();
	void reserveSpriteSpace();
};

}

#endif /* GENTMXRENDERER_HPP_ */
