#include <easylogging++.h>

#include "APG/tiled/PackedTmxRenderer.hpp"
#include "APG/internal/Assert.hpp"

namespace APG {

PackedTmxRenderer::PackedTmxRenderer(const std::string &filename, SpriteBatch *batch, int texWidth, int texHeight) :
		map{std::make_unique<Tmx::Map>()},
		packedTexture{texWidth, texHeight},
		batch{batch} {
	auto logger = el::Loggers::getLogger("APG");
	map->ParseFile(filename);

	if (map->HasError()) {
		logger->error("Failed to load TMX map %v: %v", filename, map->GetErrorText());
	}

	loadTilesets();
	loadObjects();
}

PackedTmxRenderer::PackedTmxRenderer(std::unique_ptr<Tmx::Map> &&map, SpriteBatch *batch, int texWidth, int texHeight) :
		map{std::move(map)},
		packedTexture{texWidth, texHeight},
		batch{batch} {
	loadTilesets();
	loadObjects();
}

void PackedTmxRenderer::loadTilesets() {
	auto logger = el::Loggers::getLogger("APG");

	const auto mapTileWidth = map->GetTileWidth();
	const auto mapTileHeight = map->GetTileHeight();

	for (const auto &mapTileset : map->GetTilesets()) {
		logger->info("Loading tileset %v with first GID %v", mapTileset->GetName(), mapTileset->GetFirstGid());
		REQUIRE(mapTileset->GetTileWidth() == mapTileWidth &&
				mapTileset->GetTileHeight() == mapTileHeight,
				"Only tilesets with tile size equal to map tile size are supported.");

		const auto tilesetName = map->GetFilepath() + mapTileset->GetImage()->GetSource();
		auto possibleRect = packedTexture.insertFile(tilesetName);

		if (!possibleRect) {
			logger->error(
					"Failed to pack tileset %v. Likely the specified size is not enough, or the file couldn't be found.",
					tilesetName);
			continue;
		}

		auto rect = *possibleRect;

		const auto spacing = mapTileset->GetSpacing();
		const auto tilesetTileWidth = mapTileset->GetTileWidth();
		const auto tilesetTileHeight = mapTileset->GetTileHeight();

		logger->verbose(7, "%v pack: (x, y, w, h) = (%v, %v, %v, %v)", mapTileset->GetName(), rect.x, rect.y, rect.w,
						rect.h);

		int32_t tileId = 0;
		int32_t x = 0, y = 0;
		while (true) {
			const auto tileGID = mapTileset->GetFirstGid() + tileId;
			loadedSprites.emplace_front(&packedTexture, x + rect.x, y + rect.y, tilesetTileWidth, tilesetTileHeight);
			auto &sprite = loadedSprites.front();

			sprites.insert({std::pair<int, SpriteBase *>(tileGID, &sprite)});

			logger->verbose(9, "Loaded sprite %v", tileGID);

			x += tilesetTileWidth + spacing;
			++tileId;

			if (x >= mapTileset->GetImage()->GetWidth()) {
				x = 0;
				y += tilesetTileHeight + spacing;

				if (y >= mapTileset->GetImage()->GetHeight()) {
					break;
				}
			}
		}

		// iterate over "special tiles" (we mostly care about animated tiles)
		for (const auto &tile : mapTileset->GetTiles()) {
			if (tile->IsAnimated()) {
				logger->info("Loading animated tile with %v frames and total duration of %v",
							 tile->GetFrameCount(), tile->GetTotalDuration());

				const auto &frames = tile->GetFrames();
				std::vector<SpriteBase *> framePointers;
				framePointers.reserve(frames.size());
				const float totalDuration = tile->GetTotalDuration() / 1000.0f;

				for (const auto &frame : frames) {
					const auto gid = mapTileset->GetFirstGid() + frame.GetTileID();
					const auto duration = frame.GetDuration();

					// TODO: Proper frame length handling (AnimSprite refactor)

					if (sprites.find(gid) == sprites.end()) {
						logger->error("Couldn't find sprite frame %v for animation", gid);
						continue;
					}

					framePointers.emplace_back(sprites[gid]);
				}

				loadedAnimatedSprites.emplace_front(totalDuration, framePointers, AnimationMode::LOOP);
				sprites[mapTileset->GetFirstGid() + tile->GetId()] = &(loadedAnimatedSprites.front());
			}

		}
	}

	packedTexture.commitPack();
}

void PackedTmxRenderer::loadObjects() {
	const auto logger = el::Loggers::getLogger("APG");

	for (auto &group : map->GetObjectGroups()) {
		logger->info("Loading object group \"%v\" with %v objects.", group->GetName(), group->GetNumObjects());
		std::vector<TiledObject> objects;

		for (auto &obj : group->GetObjects()) {
			const auto gid = obj->GetGid();

			if (gid == 0) {
				logger->verbose(5, "Ignoring non-tile object \"%v\"", obj->GetName());
				continue;
			}

			objects.emplace_back(obj->GetX(), obj->GetY() - map->GetTileHeight(), sprites[gid]);
		}

		objectGroups.emplace(group->GetName(), objects);
	}
}

void PackedTmxRenderer::update(float deltaTime) {
	for (auto &animation : loadedAnimatedSprites) {
		animation.update(deltaTime);
	}
}

void PackedTmxRenderer::renderAll() {
	batch->begin();

	for (const auto &layer : map->GetLayers()) {
		if (layer->IsVisible()) {
			switch (layer->GetLayerType()) {
				case Tmx::LayerType::TMX_LAYERTYPE_TILE: {
					renderLayer(static_cast<Tmx::TileLayer *>(layer));
					break;
				}

				case Tmx::LayerType::TMX_LAYERTYPE_OBJECTGROUP: {
					auto found = objectGroups.find(layer->GetName());
					if (found != objectGroups.end()) {
						renderObjectGroup(found->second);
					}
					break;
				}

				default: {
					el::Loggers::getLogger("APG")->warn("Unsupported layer type for layer \"%v\"",
														layer->GetName());
					break;
				}
			}
		}
	}

	batch->end();
}

void PackedTmxRenderer::renderAllAndUpdate(float deltaTime) {
	update(deltaTime);
	renderAll();
}

void PackedTmxRenderer::renderLayer(Tmx::TileLayer *layer) {
	const auto tileWidth = map->GetTileWidth();
	const auto tileHeight = map->GetTileHeight();

	for (int y = 0; y < layer->GetHeight(); y++) {
		for (int x = 0; x < layer->GetWidth(); x++) {
			//const uint32_t tileX = position.x + x * tileWidth;
			//const uint32_t tileY = position.y + y * tileHeight;
			const auto tileX = position.x + x * tileWidth;
			const auto tileY = position.y + y * tileHeight;

			const auto &tile = layer->GetTile(x, y);

			if (tile.tilesetId == -1) {
				continue;
			}

			const auto tileHash = tile.gid;

			const auto ourSprite = sprites.find(tileHash);

			if (ourSprite == sprites.end()) {
				el::Loggers::getLogger("APG")->error("Couldn't find sprite %v", tileHash);
				continue;
			}
			batch->draw(ourSprite->second, tileX, tileY);
		}
	}
}

void PackedTmxRenderer::renderObjectGroup(const std::vector<TiledObject> &objects) {
	for (const auto &obj : objects) {
		batch->draw(obj.sprite, position.x + obj.position.x, position.y + obj.position.y);
	}
}

PackedTexture *PackedTmxRenderer::getPackedTexture() {
	return &packedTexture;
}

const glm::vec2 &PackedTmxRenderer::getPosition() const {
	return position;
}

void PackedTmxRenderer::setPosition(glm::vec2 &position) {
	this->position = position;
}

int PackedTmxRenderer::getPixelWidth() const {
	return map->GetWidth() * map->GetTileWidth();
}

int PackedTmxRenderer::getPixelHeight() const {
	return map->GetHeight() * map->GetTileHeight();
}

}