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
		logger{spdlog::get("APG")} {
	this->preservedSurface = SXXDL::make_surface_ptr(
			SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32));
	this->packTree = std::make_unique<PackNode>(PackNode::createTree(preservedSurface.get()));
	this->setWidth(width);
	this->setHeight(height);
}

PackedTexture::~PackedTexture() {
	if(packBuffer.size() > 0) {
		logger->warn("Non-empty pack-buffer when destroying PackedTexture");
	}
	clearPackBuffer();
}

void PackedTexture::commitPack() {
	// TODO: packBuffer sorting?
	if (packBuffer.empty()) {
		logger->info("Not uploading texture as there's nothing to do");
		return;
	}

	for (auto &p : packBuffer) {
		auto surface = std::get<SDL_Surface *>(p);
		auto rect = std::get<SDL_Rect *>(p);

		SDL_BlitSurface(surface, nullptr, preservedSurface.get(), rect);
	}

	loadTexture(preservedSurface.get(), false);
	clearPackBuffer();
}

shim::optional<SDL_Rect> PackedTexture::insertFile(const std::string &filename) {
	auto surface = IMG_Load(filename.c_str());

	if (surface == nullptr) {
		logger->error("Failed to load {}; it won't be packed. Error: {}", filename, IMG_GetError());
		return shim::nullopt;
	}

	auto node = packTree->insert(surface);

	if (node == nullptr) {
		logger->error("Failed to pack {} into tree; it likely doesn't fit", filename);
		SDL_FreeSurface(surface);
		return shim::nullopt;
	}

	logger->info("Successfully packed {} into PackedTexture", filename);

	packBuffer.emplace_back(std::make_tuple(surface, node->getRegion(), true));

	return {*node->getRegion()};
}

shim::optional<SDL_Rect> PackedTexture::insertSurface(SDL_Surface *surface) {
	auto node = packTree->insert(surface);

	if (node == nullptr) {
		logger->error("Failed to pack {}x{} surface into tree; it likely doesn't fit", surface->w, surface->h);
		return shim::nullopt;
	}

	logger->info("Successfully packed {}x{} surface into PackedTexture", surface->w, surface->h);

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