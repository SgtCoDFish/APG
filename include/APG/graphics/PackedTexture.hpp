#ifndef APG_GRAPHICS_PACKEDTEXTURE_HPP
#define APG_GRAPHICS_PACKEDTEXTURE_HPP

#include <cstdint>

#include <array>
#include <vector>
#include <utility>
#include <tuple>

#include "spdlog/spdlog.h"

#include "APG/core/Optional.hpp"
#include "APG/graphics/Texture.hpp"
#include "APG/SXXDL.hpp"

namespace APG {

class PackNode {
public:
	static PackNode createTree(SDL_Surface *packTarget) {
		return PackNode(0, 0, packTarget->w, packTarget->h);
	}

	PackNode *insert(SDL_Surface *src);

	SDL_Rect *getRegion() {
		return &region;
	}

private:
	explicit PackNode(int x, int y, int w, int h);

	bool isLeaf() const;

	std::array<std::unique_ptr<PackNode>, 2> children;

	SDL_Surface *surface;
	SDL_Rect region;
};


class PackedTexture final : public Texture {
public:
	explicit PackedTexture(int32_t width, int32_t height);

	~PackedTexture() override;

	/**
	 * Packs (blits) all pending surfaces into the packed surface, and uploads the resulting texture
	 * to graphics memory
	 */
	void commitPack();

	/**
	 * Load a file and pack it into this texture (if possible)
	 * NB: The surface won't actually be packed until commitPack is called
	 * @returns the rect that can be used to create a sprite after committing
	 */
	shim::optional<SDL_Rect> insertFile(const std::string &filename);

	/**
	 * Inserts an SDL surface into this texture if possible
	 * NB: The surface won't actually be packed until commitPack is called
	 * @returns the rect that can be used to create a sprite after committing
	 */
	shim::optional<SDL_Rect> insertSurface(SDL_Surface *surface);

private:
	using surface_vec_tuple = std::tuple<SDL_Surface *, SDL_Rect *, bool>;

	std::unique_ptr<PackNode> packTree;
	// the surface to be packed, a rect describing where it should be packed,
	// and whether we own that surface (i.e. if it needs to be free'd)
	std::vector<surface_vec_tuple> packBuffer;

	void clearPackBuffer();

	std::shared_ptr<spdlog::logger> logger;
};

}

#endif
