/*
 * Copyright (c) 2014, 2015 Ashley Davis (SgtCoDFish)
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ANIMATED_SPRITE__INCLUDE
#define ANIMATED_SPRITE__INCLUDE

#include <cstdint>

#include <initializer_list>
#include <vector>

#include "APG/SpriteBase.hpp"
#include "APG/Sprite.hpp"

namespace Tmx {
class Tile;
}

namespace APG {

enum class AnimationMode {
	NORMAL, REVERSED, LOOP, LOOP_PINGPONG
};

class AnimatedSprite: public SpriteBase {
private:
	int32_t currentFrame = 0;
	int32_t frameCount = 0;

	float secondsPerFrame = 0.0f;
	float animTime = 0.0f;
	int animDir = 1;

	uint32_t width = 0, height = 0;

	Texture * texture = nullptr;

	AnimationMode animationMode;

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

	void initializeAnimFromOwned();

	/**
	 * Called by AnimatedSprite::fromTexture
	 */
	explicit AnimatedSprite(float frameDuration, std::vector<Sprite> &&sprites, AnimationMode animationMode =
	        AnimationMode::NORMAL);

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
	static std::vector<Sprite> splitTexture(Texture * texture, uint32_t tileWidth, uint32_t tileHeight,
	        uint32_t xStart = 0u, uint32_t yStart = 0u, int32_t frameCount = -1, uint32_t xSeparation = 0u);

	static std::vector<Sprite> splitTexture(const std::unique_ptr<Texture> &texture, uint32_t tileWidth,
	        uint32_t tileHeight, uint32_t xStart = 0u, uint32_t yStart = 0u, int32_t frameCount = -1,
	        uint32_t xSeparation = 0u) {
		return std::move(splitTexture(texture.get(), tileWidth, tileHeight, xStart, yStart, frameCount, xSeparation));
	}

	explicit AnimatedSprite(float frameDuration, Sprite * firstFrame, AnimationMode animationMode =
	        AnimationMode::NORMAL);
	explicit AnimatedSprite(float frameDuration, std::initializer_list<SpriteBase *> &sprites,
	        AnimationMode animationMode = AnimationMode::NORMAL);
	explicit AnimatedSprite(float frameDuration, std::vector<Sprite> &sprites, AnimationMode animationMode =
	        AnimationMode::NORMAL);
	explicit AnimatedSprite(float frameDuration, std::vector<SpriteBase *> &sprites, AnimationMode animationMode =
	        AnimationMode::NORMAL);

	virtual ~AnimatedSprite() = default;

	void update(float deltaTime);

	void addFrame(SpriteBase * frame);

	SpriteBase *getCurrentFrame() const;
	SpriteBase *getFrame(uint32_t frameNumber) const;

	inline AnimationMode getAnimationMode() const {
		return animationMode;
	}

	virtual Texture * getTexture() const override {
		return texture;
	}

	virtual uint32_t getWidth() const override {
		return width;
	}

	virtual uint32_t getHeight() const override {
		return height;
	}

	virtual float getU() const override {
		return getCurrentFrame()->getU();
	}

	virtual float getV() const override {
		return getCurrentFrame()->getV();
	}

	virtual float getU2() const override {
		return getCurrentFrame()->getU2();
	}

	virtual float getV2() const override {
		return getCurrentFrame()->getV2();
	}

	AnimatedSprite &setAnimationMode(AnimationMode mode);
};

}

#endif

