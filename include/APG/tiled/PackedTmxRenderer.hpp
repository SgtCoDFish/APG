#ifndef APG_TILED_PACKEDTMXRENDERER_HPP
#define APG_TILED_PACKEDTMXRENDERER_HPP

#include <string>
#include <memory>
#include <forward_list>

#include "Tmx.h"

#include "spdlog/spdlog.h"

#include "APG/graphics/Sprite.hpp"
#include "APG/graphics/SpriteBatch.hpp"
#include "APG/graphics/AnimatedSprite.hpp"
#include "APG/graphics/PackedTexture.hpp"

#include "APG/tiled/TiledObject.hpp"

namespace Tmx {
class Tile;

class TileLayer;
}

namespace APG {

class PackedTmxRenderer final {
public:
	explicit PackedTmxRenderer(std::unique_ptr<Tmx::Map> &&map, SpriteBatch *batch, int texWidth, int texHeight);

	explicit PackedTmxRenderer(const std::string &filename, SpriteBatch *batch, int texWidth, int texHeight);

	~PackedTmxRenderer() = default;

	void update(float deltaTime);

	void renderAll();

	void renderAllAndUpdate(float deltaTime);

	void renderLayer(Tmx::TileLayer *layer);

	void renderObjectGroup(const std::vector<TiledObject> &objects);

	const glm::vec2 &getPosition() const;

	void setPosition(glm::vec2 position);

	PackedTexture *getPackedTexture();

	int getPixelWidth() const;

	int getPixelHeight() const;

	std::vector<TiledObject> getObjectGroup(const std::string &groupName) const;

	const Tmx::Map *getMap() const;

private:
	void loadTilesets();

	void loadObjects();

	std::unique_ptr<Tmx::Map> map;
	std::unique_ptr<PackedTexture> packedTexture;

	SpriteBatch *batch;

	std::unordered_map<int, SpriteBase *> sprites;

	std::forward_list<Sprite> loadedSprites;
	std::forward_list<AnimatedSprite> loadedAnimatedSprites;

	std::unordered_map<std::string, std::vector<TiledObject>> objectGroups;

	glm::vec2 position{0, 0};

	std::shared_ptr<spdlog::logger> logger;
};

}

#endif
