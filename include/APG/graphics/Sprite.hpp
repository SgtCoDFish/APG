#ifndef INCLUDE_APG_GRAPHICS_SPRITE_HPP_
#define INCLUDE_APG_GRAPHICS_SPRITE_HPP_

#ifndef APG_NO_GL

#include <cstdint>

#include <memory>

#include "spdlog/spdlog.h"

#include "APG/graphics/Buffer.hpp"
#include "APG/graphics/SpriteBase.hpp"

namespace APG {

class Texture;

class Sprite : public SpriteBase {
public:
	explicit Sprite(const std::unique_ptr<Texture> &texture) :
			Sprite(texture.get()) {
	}

	explicit Sprite(const std::unique_ptr<Texture> &texture, int32_t texX, int32_t texY, int32_t width,
					int32_t height) :
			Sprite(texture.get(), texX, texY, width, height) {
	}

	explicit Sprite(Texture *texture);

	explicit Sprite(Texture *texture, int32_t texX, int32_t texY, int32_t width, int32_t height);

	~Sprite() override = default;

	Sprite(const Sprite &sprite) = default;

	Sprite(Sprite &&sprite) = default;

	Sprite &operator=(const Sprite &sprite) = default;

	Sprite &operator=(Sprite &&sprite) = default;

	Texture *getTexture() const override {
		return texture;
	}

	int32_t getWidth() const override {
		return width;
	}

	int32_t getHeight() const override {
		return height;
	}

	float getU1() const override {
		return u1;
	}

	float getV1() const override {
		return v1;
	}

	float getU2() const override {
		return u2;
	}

	float getV2() const override {
		return v2;
	}

protected:
	Texture *texture = nullptr;

	int32_t texX = 0, texY = 0;

	int32_t width = 0, height = 0;

	float u1 = 0.0f, v1 = 0.0f;
	float u2 = 0.0f, v2 = 0.0f;

	void calculateUV();

	std::shared_ptr<spdlog::logger> logger;
};

}

#endif

#endif /* SPRITE_HPP_ */
