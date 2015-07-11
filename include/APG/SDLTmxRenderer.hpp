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

#ifndef TMXSDLRENDERER_HPP_
#define TMXSDLRENDERER_HPP_

#include <vector>

#include "APG/APGCommon.hpp"
#include "APG/SXXDL.hpp"
#include "APG/TmxRenderer.hpp"

#include <glm/vec2.hpp>

namespace Tmx {
class Tileset;
class Layer;
}

namespace APG {

/**
 * Contains methods for rendering a loaded TMX file using SDL2.
 *
 * Requires that SDL2 + SDL2_image have already been initialised. Doesn't yet support animated tiles.
 */
class SDLTmxRenderer final : public APG::TmxRenderer {
private:
	const SXXDL::renderer_ptr &renderer;

	std::vector<SXXDL::sdl_texture_ptr> sdlTextures;

public:
	explicit SDLTmxRenderer(const std::unique_ptr<Tmx::Map> &map, const SXXDL::renderer_ptr &renderer) :
			SDLTmxRenderer(map.get(), renderer) {
	}

	explicit SDLTmxRenderer(Tmx::Map * const map, const SXXDL::renderer_ptr &renderer);
	virtual ~SDLTmxRenderer() = default;

	void renderLayer(Tmx::TileLayer * const layer) override;
	void renderObjectGroup(const std::vector<TiledObject> &objects) override;

	// disallow copying because we own resources.
	SDLTmxRenderer(SDLTmxRenderer &other) = delete;
	SDLTmxRenderer(const SDLTmxRenderer &other) = delete;
};

}

#endif /* TMXSDLRENDERER_HPP_ */
