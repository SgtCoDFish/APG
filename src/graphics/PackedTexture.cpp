#include <SDL2/SDL.h>

#include "APG/internal/Assert.hpp"
#include "APG/graphics/PackedTexture.hpp"

namespace APG {

PackNode::PackNode(int x, int y, int w, int h) :
		surface{nullptr},
		region{x, y, w, h} {
}

PackNode *PackNode::insert(SDL_Surface *src) {
	if (!isLeaf()) {
		// if we're a parent, we try to pack into child nodes
		for (auto &child : children) {
			PackNode *result = child->insert(src);
			if (result != nullptr) {
				return result;
			}
		}

		return nullptr;
	}

	// we're a leaf, so we either split or pack here
	if (this->surface != nullptr) {
		// there's already something here, we can't split again
		return nullptr;
	}

	if (src->w > region.w || src->h > region.h) {
		// the src image is too large for us to fit
		return nullptr;
	}

	// if it's a perfect fit, pack here and we're done
	if (src->w == region.w && src->h == region.h) {
		this->surface = src;
		return this;
	}

	// smaller than this region, so we split into 2 with the first child
	// being either the perfect width or height for src to fit
	const auto dw = region.w - src->w;
	const auto dh = region.h - src->h;

	if (dw >= dh) {
		children[0] = std::unique_ptr<PackNode>(new PackNode(region.x, region.y, src->w, region.h));
		children[1] = std::unique_ptr<PackNode>(new PackNode(region.x + src->w, region.y, dw, region.h));
	} else {
		children[0] = std::unique_ptr<PackNode>(new PackNode(region.x, region.y, region.w, src->h));
		children[1] = std::unique_ptr<PackNode>(new PackNode(region.x, region.y + src->h, region.w, dh));
	}

	return children[0]->insert(src);
}

bool PackNode::isLeaf() const {
	return (children[0] == nullptr);
}

PackedTexture::PackedTexture(int32_t width, int32_t height) :
		Texture(),
		workingSurface{SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32)},
		packTree{PackNode::createTree(workingSurface)} {
}

PackedTexture::~PackedTexture() {
	if (workingSurface != nullptr) {
		SDL_FreeSurface(workingSurface);
	}

	clearPackBuffer();
}

void PackedTexture::commitPack() {
	// TODO: packBuffer sorting, blitting + texture uploading goes to here.
	auto logger = el::Loggers::getLogger("APG");

	if (packBuffer.empty()) {
		logger->info("Not uploading texture as there's nothing to do");
		return;
	}

	for (auto &p : packBuffer) {
		auto surface = std::get<SDL_Surface *>(p);
		auto rect = std::get<SDL_Rect *>(p);

		SDL_BlitSurface(surface, nullptr, workingSurface, rect);
	}

	loadTexture(workingSurface);
	clearPackBuffer();
}

shim::optional<SDL_Rect> PackedTexture::insertFile(const std::string &filename) {
	auto logger = el::Loggers::getLogger("APG");
	auto surface = IMG_Load(filename.c_str());

	if (surface == nullptr) {
		logger->error("Failed to load %v; it won't be packed. Error: %v", filename, IMG_GetError());
		return shim::nullopt;
	}

	auto node = packTree.insert(surface);

	if (node == nullptr) {
		logger->error("Failed to pack %v into tree; it likely doesn't fit", filename);
		SDL_FreeSurface(surface);
		return shim::nullopt;
	}

	logger->info("Successfully packed %v into PackedTexture", filename);

	packBuffer.emplace_back(std::make_tuple(surface, node->getRegion(), true));

	return {*node->getRegion()};
}

shim::optional<SDL_Rect> PackedTexture::insertSurface(SDL_Surface *surface) {
	auto logger = el::Loggers::getLogger("APG");
	auto node = packTree.insert(surface);

	if (node == nullptr) {
		logger->error("Failed to pack %vx%v surface into tree; it likely doesn't fit", surface->w, surface->h);
		return shim::nullopt;
	}

	logger->info("Successfully packed %vx%v surface into PackedTexture", surface->w, surface->h);

	packBuffer.emplace_back(std::make_tuple(surface, node->getRegion(), false));
	return {*node->getRegion()};

}

void PackedTexture::clearPackBuffer() {
	for (auto &p : packBuffer) {
		auto surface = std::get<SDL_Surface *>(p);
		auto owned = std::get<bool>(p);
		if (surface != nullptr && owned) {
			SDL_FreeSurface(surface);
		}
	}

	packBuffer.clear();
}

}