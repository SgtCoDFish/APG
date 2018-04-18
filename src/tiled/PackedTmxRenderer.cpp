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

	const auto mapTileWidth = map->GetTileWidth();
	const auto mapTileHeight = map->GetTileHeight();

	for (const auto &mapTileset : map->GetTilesets()) {
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

		int32_t tileId = 0;
		int32_t x = rect.x, y = rect.y;
		while(true) {
			const auto tileGID = mapTileset->GetFirstGid() + tileId;
			loadedSprites.emplace_back(&packedTexture, x, y, tilesetTileWidth, tilesetTileHeight);
			auto &sprite = loadedSprites.back();

			sprites.insert(std::pair<uint64_t, SpriteBase *>(tileGID, &sprite));

			x += tilesetTileWidth + spacing;
			++tileId;

			if (x >= mapTileset->GetImage()->GetWidth()) {
				x = rect.x;
				y += tilesetTileHeight + spacing;

				if (y >= mapTileset->GetImage()->GetHeight()) {
					break;
				}
			}
		}
	}

	packedTexture.commitPack();
}

}