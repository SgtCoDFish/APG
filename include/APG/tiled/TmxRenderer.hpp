#ifndef INCLUDE_APG_TILED_TMXRENDERER_HPP_
#define INCLUDE_APG_TILED_TMXRENDERER_HPP_

// TODO: Remove dependence on SDL
#ifndef APG_NO_SDL

#include <vector>
#include <unordered_map>

#include <glm/vec2.hpp>

#include "APG/SXXDL.hpp"
#include "APG/core/APGCommon.hpp"
#include "APG/graphics/Tileset.hpp"
#include "APG/graphics/AnimatedSprite.hpp"
#include "APG/internal/Assert.hpp"

namespace Tmx {
class Map;

class Tile;
}

namespace APG {

class TiledObject {
public:
	explicit TiledObject(const glm::vec2 &pos, SpriteBase *const sprite) :
			position{pos},
			sprite{sprite} {
	}

	explicit TiledObject(float x, float y, SpriteBase *const sprite) :
			position{x, y},
			sprite{sprite} {
	}

	glm::vec2 position;
	SpriteBase *sprite;
};

/**
 * Abstracts a renderer for a TMX file that can be loaded using the tmxparser library.
 *
 * Note that sprites from tilesets are stored in a hashmap which hashes based on a uint64_t.
 * Normally this is completely transparent and you don't need to worry about it at all,
 * but it assumes that there are no more than 1,000,000 sprites in a single OpenGL texture
 * unit. This would generally be far more than you'd ever see, but things will break if
 * this happens so you should be aware.
 */
template<typename T>
class TmxRenderer {
public:
	// TAKES OWNERSHIP
	explicit TmxRenderer(std::unique_ptr<Tmx::Map> &&map) :
			ownedMap{std::move(map)} {
		init();
	}

	explicit TmxRenderer(Tmx::Map *map) :
			TmxRenderer(std::unique_ptr<Tmx::Map>(map)) {
	}

	explicit TmxRenderer(const std::string &fileName) {
		ownedMap = std::make_unique<Tmx::Map>();
		ownedMap->ParseFile(fileName);

		init();
	}

	virtual ~TmxRenderer() = default;

	void renderLayer(const Tmx::TileLayer *layer) {
		static_cast<T *>(this)->renderLayerImpl(layer);
	}

	void renderObjectGroup(const std::vector<TiledObject> &objects) {
		static_cast<T *>(this)->renderObjectGroupImpl(objects);
	}

	/**
	 * Renders all the layers and calls update for you.
	 */
	void renderAll(float deltaTime, bool autoUpdate = true) {
		if (autoUpdate) {
			update(deltaTime);
		}

		for (const auto &layer : map->GetLayers()) {
			if (layer->IsVisible()) {
				switch (layer->GetLayerType()) {
					case Tmx::LayerType::TMX_LAYERTYPE_TILE: {
						renderLayer((Tmx::TileLayer *) layer);
						break;
					}

					case Tmx::LayerType::TMX_LAYERTYPE_OBJECTGROUP: {
						renderObjectGroup(objectGroups[layer->GetName()]);
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
	}

	void update(float deltaTime) {
		for (auto &animation : loadedAnimatedSprites) {
			animation.update(deltaTime);
		}
	}

	const Tmx::Map *getMap() {
		return map;
	}

	const glm::vec2 &getPosition() const {
		return position;
	}

	void setPosition(glm::vec2 &position) {
		this->position = position;
	}

	Tileset *getTilesetByID(int32_t id) const {
		if (tilesets[id] == nullptr) {
			return nullptr;
		}

		return tilesets[id].get();
	}

	TmxRenderer(TmxRenderer &other) = delete;

	TmxRenderer(const TmxRenderer &other) = delete;

	TmxRenderer &operator=(TmxRenderer &other) = delete;

	TmxRenderer &operator=(const TmxRenderer &other) = delete;

protected:
	static const uint64_t MAX_SPRITES_PER_UNIT = 1000000;

	void initialiseTilesets(std::unordered_map<std::string, std::shared_ptr<Tileset>> &tmxTilesets) {
		if (tmxTilesets.empty()) {
			tmxTilesets.reserve(internal::MAX_SUPPORTED_TEXTURES);
		}
	}

	Tmx::Map *map = nullptr;
	std::vector<std::shared_ptr<Tileset>> tilesets;
	std::unordered_map<uint64_t, SpriteBase *> sprites;

	std::vector<Sprite> loadedSprites;
	std::vector<AnimatedSprite> loadedAnimatedSprites;

	std::unordered_map<std::string, std::vector<TiledObject>> objectGroups;

	glm::vec2 position{0.0f, 0.0f};

	void loadTilesets() {
		auto &tmxTilesets = getDerivedTmxTilesets();
		initialiseTilesets(tmxTilesets);

		const auto logger = el::Loggers::getLogger("APG");

		const auto tileWidth = map->GetTileWidth();
		const auto tileHeight = map->GetTileHeight();

		logger->info("Loading map %v with (tileWidth, tileHeight) = (%vpx, %vpx)", map->GetFilename(), tileWidth,
					 tileHeight);

#ifdef APG_IGNORE_ANIMATED_TILES
		logger->info("Note: APG_IGNORE_ANIMATED_TILES is set.");
#endif

		/*
		 * We need to reserve space for our sprites or the vectors will be
		 * dynamically reallocated during loading,
		 * and we'll be left with a load of broken pointers.
		 */
		reserveSpriteSpace();

		for (const auto &tileset : map->GetTilesets()) {
			REQUIRE(tileset->GetTileWidth() == tileWidth && tileset->GetTileHeight() == tileHeight,
					"Only tilesets with tile size consistent with the map are currently supported.");

			const auto tilesetName = map->GetFilepath() + tileset->GetImage()->GetSource();

			const auto tilesetExists = tmxTilesets.find(tilesetName);
			Tileset *loadedTileset = nullptr;

			if (tilesetExists != tmxTilesets.end()) {
				logger->info("Using previously loaded tileset for \"%v\"", tilesetName);

				loadedTileset = tilesetExists->second.get();
			} else {
				logger->info("Loading tileset \"%v\" (first GID = %v, has %v special tiles, spacing = %vpx)",
							 tilesetName,
							 tileset->GetFirstGid(), tileset->GetTiles().size(), tileset->GetSpacing());

				tmxTilesets.emplace(tilesetName, std::make_shared<Tileset>(tilesetName, tileset));

				tilesets.emplace_back(std::shared_ptr<Tileset>(tmxTilesets.at(tilesetName)));
				loadedTileset = tilesets.back().get();
			}

			REQUIRE(loadedTileset != nullptr, "Couldn't load/find tileset when loading map");

			const auto spacing = loadedTileset->getSpacing();

			/*
			 * Note that tileset->GetTiles() only returns tiles which have something special about them, e.g. a property or an animation.
			 *
			 * This means we need to create our Sprites by iterating through the image sequentially.
			 *
			 * The GIDs start at 1 for the first tileset, and increase by 1 going right.
			 * When the right edge of the image is reached, we go down and back to the left of the image.
			 */
			int32_t tileID = 0, x = 0, y = 0;
			while (true) {
				const auto tileGID = calculateTileGID(tileset, tileID);

				loadedSprites.emplace_back(loadedTileset, x, y, tileWidth, tileHeight);

				auto &sprite = loadedSprites.back();
				sprite.setHash(tileGID);

				sprites.insert(std::pair<uint64_t, SpriteBase *>(sprite.getHash(), &sprite));

				x += tileWidth + spacing;
				++tileID;
				if (x >= loadedTileset->getWidth()) {
					x = 0;
					y += tileHeight + spacing;

					if (y >= loadedTileset->getHeight()) {
						break;
					}
				}
			}

			for (const auto &tile : tileset->GetTiles()) {
				const auto tileGID = calculateTileGID(tileset, tile);

				logger->verbose(1, "Special tile #%v has %v properties.", tileGID, tile->GetProperties().GetSize());

				if (el::Loggers::verboseLevel() >= 1) {
					for (const auto &property : tile->GetProperties().GetList()) {
						logger->verbose(1, R"(Property "%v" = "%v")", property.first, property.second);
					}
				}

#ifndef APG_IGNORE_ANIMATED_TILES
				if (tile->IsAnimated()) {
					logger->verbose(1, "Animated tile has %v tiles, with total duration %vms.", tile->GetFrameCount(),
									tile->GetTotalDuration());

					const auto &frames = tile->GetFrames();
					std::vector<SpriteBase *> framePointers;
					framePointers.reserve(frames.size());

					// TODO: This ignores per-frame durations in the file, should be improved.
					// Length is stored in ms, convert to seconds
					float length = (float) tile->GetTotalDuration() / 1000.0f;
					unsigned int framesLoaded = 0;

					for (const auto &frame : frames) {
						const auto gid = calculateTileGID(tileset, frame.GetTileID());
						logger->verbose(2, "Frame #%v: ID %v, GID %v, duration = %vms.", framesLoaded,
										frame.GetTileID(),
										gid, frame.GetDuration());

						REQUIRE(sprites.find(gid) != sprites.end(), "Animation frame not loaded into sprites array.");

						framePointers.emplace_back(sprites[gid]);
						++framesLoaded;
					}

					loadedAnimatedSprites.emplace_back(length, framePointers, AnimationMode::LOOP);
					sprites[tileGID] = &(loadedAnimatedSprites.back());
				}
#endif
			}
		}
	}

	void loadObjects() {
		const auto logger = el::Loggers::getLogger("APG");

		for (auto &group : map->GetObjectGroups()) {
			logger->info("Loading object group \"%v\", has %v objects.", group->GetName(), group->GetNumObjects());
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

	uint64_t calculateTileGID(Tmx::Tileset *tileset, int tileID) {
		return tileset->GetFirstGid() + tileID;
	}

	uint64_t calculateTileGID(Tmx::Tileset *tileset, Tmx::Tile *tile) {
		return calculateTileGID(tileset, tile->GetId());
	}

private:
	std::unique_ptr<Tmx::Map> ownedMap;

	void init() {
		map = ownedMap.get();

		loadTilesets();
		loadObjects();
	}

	void reserveSpriteSpace() {
		int32_t tileCount = 0;
		int32_t animTileCount = 0;

		const auto &tilesets = map->GetTilesets();

// Go through the tilesets counting all the static tiles and then individually counting animated tiles.
		for (const auto &tileset : tilesets) {
			tileCount += (tileset->GetImage()->GetWidth() / tileset->GetTileWidth())
						 * (tileset->GetImage()->GetHeight() / tileset->GetTileHeight());

			// Find animated tiles; there are probably far fewer animated than static, so reserving the same amount of space would be wasteful.
			for (const auto &tile : tileset->GetTiles()) {
				if (tile->IsAnimated()) {
					++animTileCount;
				}
			}
		}

		sprites.reserve(tileCount);
		loadedSprites.reserve(tileCount);
		loadedAnimatedSprites.reserve(animTileCount);
	}

	std::unordered_map<std::string, std::shared_ptr<Tileset>> & getDerivedTmxTilesets() {
		return static_cast<T*>(this)->getTmxTilesets();
	}
};

}

#endif

#endif /* TMXRENDERER_HPP_ */
