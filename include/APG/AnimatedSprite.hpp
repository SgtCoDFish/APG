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
	uint64_t currentFrame = 0;
	uint64_t frameCount;

	float secondsPerFrame = 0.0f;
	float animTime = 0.0f;
	int animDir = 1;

	AnimationMode animationMode;

	Sprite firstFrame;
	std::vector<Sprite *> frames;

	void handleNormalMode_();
	void handleLoopMode_();
	void handleLoopPingPongMode_();

public:
	AnimatedSprite(float frameDuration, Sprite &&firstFrame, std::initializer_list<Sprite *> sprites,
			AnimationMode animationMode = AnimationMode::NORMAL);
	AnimatedSprite(float frameDuration, Sprite &&firstFrame,  std::vector<Sprite *> sprites,
			AnimationMode animationMode = AnimationMode::NORMAL);

	void update(float deltaTime);

	Sprite *getCurrentFrame() const;
	Sprite *getFrame(uint16_t frameNumber) const;

	inline AnimationMode getAnimationMode() const {
		return animationMode;
	}

	virtual Texture * getTexture() const override {
		return getCurrentFrame()->getTexture();
	}

	virtual inline int32_t getWidth() const override {
		return getCurrentFrame()->getWidth();
	}

	virtual inline int32_t getHeight() const override {
		return getCurrentFrame()->getHeight();
	}

	virtual inline float getU() const override {
		return getCurrentFrame()->getU();
	}

	virtual inline float getV() const override {
		return getCurrentFrame()->getV();
	}

	virtual inline float getU2() const override {
		return getCurrentFrame()->getU2();
	}

	virtual inline float getV2() const override {
		return getCurrentFrame()->getV2();
	}

	void setAnimationMode(AnimationMode mode);
};

}

#endif

