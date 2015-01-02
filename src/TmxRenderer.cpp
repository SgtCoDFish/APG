/*
 * TmxRenderer.cpp
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

#include <vector>

#include "TmxMap.h"
#include "TmxLayer.h"
#include "TmxTileset.h"
#include "TmxImage.h"

#include "APGCommon.hpp"
#include "ErrorBase.hpp"
#include "Tileset.hpp"
#include "SXXDL.hpp"
#include "TmxRenderer.hpp"

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

