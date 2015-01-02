/*
 * SDLTmxRenderer.hpp
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

#ifndef TMXSDLRENDERER_HPP_
#define TMXSDLRENDERER_HPP_

#include <vector>

#include "APGCommon.hpp"
#include "ErrorBase.hpp"
#include "SXXDL.hpp"
#include "TmxRenderer.hpp"

#include <glm/vec2.hpp>

namespace Tmx {
class Tileset;
class Layer;
}

namespace APG {

/**
 * Contains methods for rendering a loaded TMX file using SDL2.
 *
 * Requires that SDL2/SDL2_image have already been initialised.
 */
class SDLTmxRenderer : public APG::TmxRenderer {
private:
	SXXDL::renderer_ptr &renderer;

	std::vector<SXXDL::sdl_texture_ptr> sdlTextures;

public:
	SDLTmxRenderer(Tmx::Map * const map, SXXDL::renderer_ptr &renderer);

	void renderLayer(Tmx::Layer *layer) override;

	// disallow copying because we own resources.
	SDLTmxRenderer(SDLTmxRenderer &other) = delete;
	SDLTmxRenderer(const SDLTmxRenderer &other) = delete;
};

}

#endif /* TMXSDLRENDERER_HPP_ */
