/*
 * TmxRenderer.hpp
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

#ifndef GENTMXRENDERER_HPP_
#define GENTMXRENDERER_HPP_

#include <vector>

#include "APGCommon.hpp"
#include "ErrorBase.hpp"
#include "Tileset.hpp"
#include "SXXDL.hpp"

#include <glm/vec2.hpp>

namespace APG {

class TmxRenderer : public ErrorBase {
protected:
	map_ptr &map;
	std::vector<tileset_ptr> tilesets;

	glm::vec2 position { 0.0f, 0.0f };

	void loadTilesets();

public:
	explicit TmxRenderer(map_ptr &map);

	virtual ~TmxRenderer() {
	}

	virtual void renderLayer(Tmx::Layer *layer) = 0;
	void renderAll();

	const Tmx::Map *getMap() {
		return map.get();
	}

	const glm::vec2 &getPosition() const {
		return position;
	}

	void setPosition(glm::vec2 &position) {
		this->position = position;
	}

	Tileset * const getTilesetByID(int32_t id) const {
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
