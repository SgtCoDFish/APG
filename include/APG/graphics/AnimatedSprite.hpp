#ifndef INCLUDE_GRAPHICS_ANIMATED_SPRITE
#define INCLUDE_GRAPHICS_ANIMATED_SPRITE

#ifndef APG_NO_SDL
#ifndef APG_NO_GL

#include <cstdint>

#include <initializer_list>
#include <vector>

#include "APG/graphics/SpriteBase.hpp"
#include "APG/graphics/Sprite.hpp"

namespace Tmx {
class Tile;
}

namespace APG {

enum class AnimationMode {
	NORMAL, REVERSED, LOOP, LOOP_PINGPONG
};

class AnimatedSprite final : public SpriteBase {
public:
	/**
	 * Splits a texture into sprite frames, ready to be used by an animated sprite.
	 *
	 * Frames are loaded left to right.
	 *
	 * @param texture The texture containing the frames.
	 * @param tileWidth The width, in pixels, of a frame.
	 * @param tileHeight The height, in pixels, of a frame.
	 * @param xStart The starting x position to load from, defaults to 0.
	 * @param yStart The starting y position to load from, defaults to 0.
	 * @param frameCount The number of frames to use. Defaults to -1; use -1 to use every available frame that fits.
	 * @param xSeparation The horizontal separation between two frames, in pixels, defaults to 0.
	 * @return a list of sprites that can be passed into an AnimatedSprite constructor
	 */
	static std::vector<Sprite> splitTexture(Texture * texture, int32_t tileWidth, int32_t tileHeight,
	        int32_t xStart = 0, int32_t yStart = 0, int32_t frameCount = -1, int32_t xSeparation = 0);

	static std::vector<Sprite> splitTexture(const std::unique_ptr<Texture> &texture, int32_t tileWidth,
	        int32_t tileHeight, int32_t xStart = 0, int32_t yStart = 0, int32_t frameCount = -1,
	        int32_t xSeparation = 0) {
		return splitTexture(texture.get(), tileWidth, tileHeight, xStart, yStart, frameCount, xSeparation);
	}

	explicit AnimatedSprite(float frameDuration, Sprite * firstFrame, AnimationMode animationMode =
	        AnimationMode::NORMAL);
	explicit AnimatedSprite(float frameDuration, std::initializer_list<SpriteBase *> &sprites,
	        AnimationMode animationMode = AnimationMode::NORMAL);
	explicit AnimatedSprite(float frameDuration, std::vector<Sprite> &sprites, AnimationMode animationMode =
	        AnimationMode::NORMAL);
	explicit AnimatedSprite(float frameDuration, std::vector<SpriteBase *> &sprites, AnimationMode animationMode =
	        AnimationMode::NORMAL);

	~AnimatedSprite() override = default;

	void update(float deltaTime);

	void addFrame(SpriteBase * frame);

	SpriteBase *getCurrentFrame() const;
	SpriteBase *getFrame(uint32_t frameNumber) const;

	inline AnimationMode getAnimationMode() const {
		return animationMode;
	}

	Texture * getTexture() const override {
		return texture;
	}

	int32_t getWidth() const override {
		return width;
	}

	int32_t getHeight() const override {
		return height;
	}

	float getU1() const override {
		return getCurrentFrame()->getU1();
	}

	float getV1() const override {
		return getCurrentFrame()->getV1();
	}

	float getU2() const override {
		return getCurrentFrame()->getU2();
	}

	float getV2() const override {
		return getCurrentFrame()->getV2();
	}

	AnimatedSprite &setAnimationMode(AnimationMode mode);

private:
	int32_t currentFrame = 0;
	int32_t frameCount = 0;

	float secondsPerFrame = 0.0f;
	float animationTime = 0.0f;
	int animationDirection = 1;

	int32_t width = 0, height = 0;

	Texture * texture = nullptr;

	AnimationMode animationMode { AnimationMode::NORMAL };

	std::vector<SpriteBase *> frames;

	inline void progress_();
	void handleNormalMode_();
	void handleLoopMode_();
	void handleLoopPingPongMode_();
	void handleReversedMode_();

	/**
	 * Sets width, height, u, v, u2, v2 and the texture pointer from the given frame.
	 * All future frames must match the width, height, and texture of this frame.
	 */
	void initializeFromSpriteFrame(const SpriteBase * sprite);
};

}

#endif
#endif

#endif

