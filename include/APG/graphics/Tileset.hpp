#ifndef INCLUDE_APG_GRAPHICS_TILESET_HPP_
#define INCLUDE_APG_GRAPHICS_TILESET_HPP_

#ifndef APG_NO_SDL
#ifndef APG_NO_GL

#include <cstdint>

#include <string>
#include <memory>

#include "Tmx.h"

#include "APG/graphics/Texture.hpp"

#include "easylogging++.h"

namespace APG {

/**
 * A tileset from which tiles can be picked. Note that if there is a non-zero spacing between tiles, this is expected to be uniformly applied; that is, it is expected that:
 * (imageWidth) % (tileWidth + spacing) == 0.
 */
class Tileset : public Texture {
public:
	/**
	 * Create a new tileset based on a Tmx Map. Normally, you'll want to use Tileset(std::string, Tmx::Tileset *) instead.
	 *
	 * Assumes that tiles are tightly packed; that is, assumes there is no spacing between tiles.
	 * @param fileName The location of the tileset to load.
	 * @param map The map whose tile widths we'll be using for this tileset.
	 */
	explicit Tileset(const std::string &fileName, Tmx::Map * const map) :
			        Tileset(fileName, map->GetTileWidth(), map->GetTileHeight()) {
	}

	/**
	 * Creates a new tileset based on a Tmx::Tileset. Correctly handles spacing of tiles.
	 *
	 * @param fileName The location of the tileset to load.
	 * @param tileset The tmx tileset describing the tileset. This information is parsed, and spacing information is used appropriately.
	 */
	explicit Tileset(const std::string &fileName, Tmx::Tileset * const tileset) :
			        Tileset(fileName, tileset->GetTileWidth(), tileset->GetTileHeight(), tileset->GetSpacing()) {
	}

	explicit Tileset(const std::string &fileName, int32_t tileWidth, int32_t tileHeight, int32_t spacing = 0) :
			        Texture { fileName },
			        tileWidth { tileWidth },
			        tileHeight { tileHeight },
			        spacing { spacing } {
		calculateWidthInTiles();
		calculateHeightInTiles();
	}

	~Tileset() override = default;

	int32_t getTileWidth() const {
		return tileWidth;
	}

	int32_t getTileHeight() const {
		return tileHeight;
	}

	int32_t getWidthInTiles() const {
		return widthInTiles;
	}

	int32_t getHeightInTiles() const {
		return heightInTiles;
	}

	int32_t getSpacing() const {
		return spacing;
	}

private:
	int32_t tileWidth = 0;
	int32_t tileHeight = 0;

	int32_t widthInTiles = 0;
	int32_t heightInTiles = 0;

	int32_t spacing = 0;

	void calculateWidthInTiles() {
		const auto gap = tileWidth + spacing;

		if (getWidth() % gap != 0) {
			el::Loggers::getLogger("APG")->warn(
			        "Tileset %v may have an inconsistent spacing in the x direction; this could cause issues.",
			        this->getFileName());
		}

		widthInTiles = getWidth() / gap;
	}

	void calculateHeightInTiles() {
		const auto gap = tileHeight + spacing;

		if (getHeight() % gap != 0) {
			el::Loggers::getLogger("APG")->warn(
			        "Tileset %v may have an inconsistent spacing in the y direction; this could cause issues.",
			        this->getFileName());
		}

		heightInTiles = getHeight() / gap;
	}
};

}

#endif
#endif

#endif /* APGTILESET_HPP_ */
