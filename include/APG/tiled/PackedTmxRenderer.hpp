#ifndef APG_TILED_PACKEDTMXRENDERER_HPP
#define APG_TILED_PACKEDTMXRENDERER_HPP

#include <string>
#include <memory>
#include <forward_list>

#include "Tmx.h"

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
	explicit PackedTmxRenderer(std::unique_ptr<Tmx::Map> &&map, SpriteBatch * batch);
	explicit PackedTmxRenderer(const std::string &filename, SpriteBatch * batch);
	~PackedTmxRenderer() = default;

	void update(float deltaTime);

	void renderAll();
	void renderAllAndUpdate(float deltaTime);

	void renderLayer(Tmx::TileLayer * layer);
	void renderObjectGroup(const std::vector<TiledObject> &objects);

	PackedTexture * getPackedTexture();

private:
	void loadTilesets();
	void loadObjects();

	std::unique_ptr<Tmx::Map> map;
	PackedTexture packedTexture;

	SpriteBatch * batch;

	std::unordered_map<int, SpriteBase *> sprites;

	std::forward_list<Sprite> loadedSprites;
	std::forward_list<AnimatedSprite> loadedAnimatedSprites;

	std::unordered_map<std::string, std::vector<TiledObject>> objectGroups;
};

}

#endif
