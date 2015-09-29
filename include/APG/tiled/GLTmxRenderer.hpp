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

#ifndef GLTMXRENDERER_HPP_
#define GLTMXRENDERER_HPP_

#include "APG/graphics/SpriteBatch.hpp"
#include "APG/tiled/TmxRenderer.hpp"

namespace Tmx {
class Layer;
}

namespace APG {

class GLTmxRenderer final : public TmxRenderer {
private:
	SpriteBatch *batch;

public:
	explicit GLTmxRenderer(const std::unique_ptr<Tmx::Map> &map, const std::unique_ptr<SpriteBatch> &batch) :
			        GLTmxRenderer(map.get(), batch.get()) {
	}

	explicit GLTmxRenderer(Tmx::Map * const map, SpriteBatch * const batch);
	virtual ~GLTmxRenderer() = default;

	virtual void renderAll(float deltaTime);

	virtual void renderLayer(Tmx::TileLayer * const layer) override;
	virtual void renderObjectGroup(const std::vector<TiledObject> &objects) override;
};

}

#endif /* GLTMXRENDERER_HPP_ */