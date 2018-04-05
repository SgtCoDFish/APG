#ifndef APG_NO_SDL
#ifndef APG_NO_GL

#include <glm/vec2.hpp>

#include "Tmx.h"

#include "APG/graphics/Sprite.hpp"
#include "APG/tiled/GLTmxRenderer.hpp"
#include "APG/internal/Assert.hpp"

namespace APG {

std::unordered_map<std::string, std::shared_ptr<Tileset>> GLTmxRenderer::tmxTilesets;

GLTmxRenderer::GLTmxRenderer(Tmx::Map *const map, SpriteBatch *const batch) :
		GLTmxRenderer(std::unique_ptr<Tmx::Map>(map), batch) {
}

GLTmxRenderer::GLTmxRenderer(std::unique_ptr<Tmx::Map> &&map, SpriteBatch *const batch) :
		TmxRenderer(std::move(map)),
		batch{batch} {
}


GLTmxRenderer::GLTmxRenderer(const std::string &fileName, SpriteBatch *const batch) :
		TmxRenderer(fileName),
		batch{batch} {

}

void GLTmxRenderer::renderAll(float deltaTime) {
	batch->begin();

	TmxRenderer<GLTmxRenderer>::renderAll(deltaTime);

	batch->end();
}

void GLTmxRenderer::renderLayerImpl(const Tmx::TileLayer *layer) {
	const uint32_t tileWidth = map->GetTileWidth();
	const uint32_t tileHeight = map->GetTileHeight();

	for (int y = 0; y < layer->GetHeight(); y++) {
		for (int x = 0; x < layer->GetWidth(); x++) {
			const uint32_t tileX = position.x + x * tileWidth;
			const uint32_t tileY = position.y + y * tileHeight;

			const auto &tile = layer->GetTile(x, y);

			if (tile.tilesetId == -1) {
				continue;
			}

			const auto tileHash = tile.gid;

			const auto ourSprite = sprites[tileHash];
			batch->draw(ourSprite, tileX, tileY);
		}
	}
}

void GLTmxRenderer::renderObjectGroupImpl(const std::vector<TiledObject> &objects) {
	for (const auto &obj : objects) {
		batch->draw(obj.sprite, obj.position.x, obj.position.y);
	}
}

}

#endif
#endif
