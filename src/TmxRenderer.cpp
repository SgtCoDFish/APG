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

#include <vector>

#include "tmxparser/TmxMap.h"
#include "tmxparser/TmxLayer.h"
#include "tmxparser/TmxTileset.h"
#include "tmxparser/TmxImage.h"

#include "APG/APGCommon.hpp"
#include "APG/ErrorBase.hpp"
#include "APG/Tileset.hpp"
#include "APG/SXXDL.hpp"
#include "APG/TmxRenderer.hpp"

#include <glm/vec2.hpp>

APG::TmxRenderer::TmxRenderer(Tmx::Map *map) :
		map { map } {
	loadTilesets();
}

void APG::TmxRenderer::loadTilesets() {
	for (const auto &tileset : map->GetTilesets()) {
		const auto tilesetName = map->GetFilepath() + tileset->GetImage()->GetSource();

		auto loadedTileset = tileset_ptr(new Tileset(tilesetName, map));

		if (loadedTileset->hasError()) {
			setErrorState(
					std::string("Couldn't load ") + tilesetName + ": "
							+ loadedTileset->getErrorMessage());
			return;
		}

		tilesets.emplace_back(std::move(loadedTileset));
	}
}

void APG::TmxRenderer::renderAll() {
	for (const auto &layer : map->GetLayers()) {
		renderLayer(layer);
	}
}

