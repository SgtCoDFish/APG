#include <easylogging++.h>

#include "APG/tiled/PackedTmxRenderer.hpp"
#include "APG/internal/Assert.hpp"

namespace APG {

PackedTmxRenderer::PackedTmxRenderer(const std::string &filename) :
		map{std::make_unique<Tmx::Map>()},
		packedTexture{2048, 2048} {
	map->ParseFile(filename);

	auto logger = el::Loggers::getLogger("APG");

	if (map->HasError()) {
		logger->error("Failed to load TMX map %v: %v", filename, map->GetErrorText());
	}

	const auto tileWidth = map->GetTileWidth();
	const auto tileHeight = map->GetTileHeight();

	for (const auto &mapTileset : map->GetTilesets()) {
		REQUIRE(mapTileset->GetTileWidth() == tileWidth &&
				mapTileset->GetTileHeight() == tileHeight,
				"Only tilesets with tile size equal to map tile size are supported.");

		const auto tilesetName = map->GetFilepath() + mapTileset->GetImage()->GetSource();
		const auto rect = packedTexture.insertFile(tilesetName);

		if (!rect) {
			logger->error(
					"Failed to pack tileset %v. Likely the specified size is not enough, or the file couldn't be found.",
					tilesetName);
			continue;
		}

		// TODO: Load tiles as sprites
	}

	packedTexture.commitPack();
}

}