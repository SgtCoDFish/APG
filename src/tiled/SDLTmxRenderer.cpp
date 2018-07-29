#ifndef APG_NO_SDL

#include <string>
#include <sstream>

#include "Tmx.h"

#include "APG/SDL.hpp"

#include "APG/SXXDL.hpp"
#include "APG/tiled/TmxRenderer.hpp"
#include "APG/tiled/SDLTmxRenderer.hpp"
#include "APG/internal/Assert.hpp"


namespace APG {

std::unordered_map<std::string, std::shared_ptr<Tileset>> SDLTmxRenderer::tmxTilesets;

SDLTmxRenderer::SDLTmxRenderer(Tmx::Map *const map, const SXXDL::renderer_ptr &renderer) :
		SDLTmxRenderer(std::unique_ptr<Tmx::Map>(map), renderer) {

}

SDLTmxRenderer::SDLTmxRenderer(std::unique_ptr<Tmx::Map> &&map, const SXXDL::renderer_ptr &renderer) :
		TmxRenderer(std::move(map)),
		renderer{renderer} {
	setupTilesets();
}

SDLTmxRenderer::SDLTmxRenderer(const std::string &fileName, const SXXDL::renderer_ptr &renderer) :
		TmxRenderer(fileName),
		renderer{renderer} {
	setupTilesets();
}

void SDLTmxRenderer::setupTilesets() {
	REQUIRE(map->GetOrientation() == Tmx::MapOrientation::TMX_MO_ORTHOGONAL,
			"SDLTmxRenderer only supports orthogonal maps.");

	for (const auto &tileset : tilesets) {
		sdlTextures.emplace_back(
				SXXDL::make_sdl_texture_ptr(
						SDL_CreateTextureFromSurface(renderer.get(), tileset->getPreservedSurface())));
	}
}

void SDLTmxRenderer::renderLayerImpl(const Tmx::TileLayer *layer) {
	const auto tile_width = map->GetTileWidth();
	const auto tile_height = map->GetTileHeight();

	auto src_rect = SDL_Rect{0, 0, tile_width, tile_height};
	auto dst_rect = SDL_Rect{0, 0, tile_width, tile_height};

	if (layer->IsVisible()) {
		for (int y = 0; y < layer->GetHeight(); y++) {
			for (int x = 0; x < layer->GetWidth(); x++) {

				const unsigned int tile_id = layer->GetTileId(x, y);
				const auto tileset_index = layer->GetTileTilesetIndex(x, y);

				if (tileset_index == -1) {
					continue;
				}

				const auto &current_tileset = tilesets[tileset_index];
				const auto &sdl_tileset = sdlTextures[tileset_index];

				const int tileset_x = tile_id % current_tileset->getWidthInTiles();
				const int tileset_y = tile_id / current_tileset->getWidthInTiles();

				const auto spacing = current_tileset->getSpacing();

				src_rect.x = tileset_x * (tile_width + spacing);
				src_rect.y = tileset_y * (tile_height + spacing);

				dst_rect.x = (int) position.x + x * tile_width;
				dst_rect.y = (int) position.y + y * tile_height;

				if (SDL_RenderCopy(renderer.get(), sdl_tileset.get(), &src_rect, &dst_rect) < 0) {
					logger->error(
							"Couldn't render in tmx renderer; failed at tile {}, tileset \"{}\"", tile_id,
							current_tileset->getFileName());
					return;
				}
			}
		}
	}
}

void SDLTmxRenderer::renderObjectGroupImpl(const std::vector<TiledObject> &objects) {
	logger->critical("SDL renderer cannot render object groups");
	throw std::runtime_error("SDL renderer cannot render object groups");
}

}

#endif
