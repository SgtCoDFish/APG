/*
 * SDLTmxRenderer.hpp
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

#ifndef TMXSDLRENDERER_HPP_
#define TMXSDLRENDERER_HPP_

#include <vector>

#include "APGCommon.hpp"
#include "ErrorBase.hpp"
#include "SDLTileset.hpp"
#include "SXXDL.hpp"

#include <glm/vec2.hpp>

namespace Tmx {
class Tileset;
}

namespace APG {

/**
 * Contains methods for rendering a loaded TMX file using SDL2.
 *
 * Requires that SDL2/SDL2_image have already been initialised.
 */
class SDLTmxRenderer : public ErrorBase {
private:
	APG::tileset_ptr loadTileset(const Tmx::Tileset *tileset);

	map_ptr &map;
	SXXDL::renderer_ptr &renderer;

	glm::vec2 position;

	std::vector<tileset_ptr> tilesets;

public:
	SDLTmxRenderer(SDLTmxRenderer &other) = delete;
	SDLTmxRenderer(const SDLTmxRenderer &other) = delete;

	SDLTmxRenderer(map_ptr &map, SXXDL::renderer_ptr &renderer);

	/**
	 * Renders all (visible) layers to the screen, starting at the renderer's x, y coordinates and going right and down.
	 */
	void renderAll();

	const glm::vec2 &getPosition() const {
		return position;
	}

	void setPosition(glm::vec2 &position) {
		this->position = position;
	}
};

}

#endif /* TMXSDLRENDERER_HPP_ */
