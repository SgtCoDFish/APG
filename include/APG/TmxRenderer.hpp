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

#include "APG/APGCommon.hpp"
#include "APG/ErrorBase.hpp"
#include "APG/Tileset.hpp"
#include "APG/SXXDL.hpp"
#include "APG/AnimatedSprite.hpp"

#include <glm/vec2.hpp>

namespace Tmx {
class Map;
}

namespace APG {

class TmxRenderer : public ErrorBase {
protected:
	Tmx::Map *map = nullptr;
	std::vector<tileset_ptr> tilesets;
	std::vector<AnimatedSprite> animatedSprites;

	glm::vec2 position { 0.0f, 0.0f };

	void loadTilesets();

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
			return tilesets[id].get();
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
