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

#include <cstdint>

#include <initializer_list>
#include <vector>
#include <utility>

#include "tmxparser/TmxTile.h"

#include "APG/AnimatedSprite.hpp"

APG::AnimatedSprite::AnimatedSprite(float frameDuration, Sprite &&firstFrame, std::initializer_list<Sprite *> sprites,
        AnimationMode animationMode) :
		frameCount { sprites.size() }, secondsPerFrame { frameDuration }, firstFrame { std::move(firstFrame) } {
	for (Sprite *sprite : sprites) {
		if (sprite == nullptr) {
			frames.emplace_back(&this->firstFrame);
		} else {
			frames.emplace_back(sprite);
		}
	}

	setAnimationMode(animationMode);
}

APG::AnimatedSprite::AnimatedSprite(float frameDuration, Sprite &&firstFrame, std::vector<Sprite *> sprites,
        AnimationMode animationMode) :
		frameCount { sprites.size() }, secondsPerFrame { frameDuration }, firstFrame { std::move(firstFrame) } {
	for (Sprite *sprite : sprites) {
		if (sprite == nullptr) {
			frames.emplace_back(&this->firstFrame);
		} else {
			frames.emplace_back(sprite);
		}
	}

	setAnimationMode(animationMode);
}

void APG::AnimatedSprite::update(float deltaTime) {
	animTime += deltaTime;

	switch (animationMode) {
	case AnimationMode::NORMAL:
	case AnimationMode::REVERSED:
		handleNormalMode_();
		break;

	case AnimationMode::LOOP:
		handleLoopMode_();
		break;

	case AnimationMode::LOOP_PINGPONG:
		handleLoopPingPongMode_();
		break;

	default:
		break;
		//
	}
}

APG::Sprite *APG::AnimatedSprite::getFrame(uint16_t frameNumber) const {
	return frames[frameNumber];
}

APG::Sprite *APG::AnimatedSprite::getCurrentFrame() const {
	return frames[currentFrame];
}

void APG::AnimatedSprite::setAnimationMode(APG::AnimationMode mode) {
	this->animationMode = mode;

	switch (this->animationMode) {
	case AnimationMode::NORMAL:
		currentFrame = 0;
		break;

	case AnimationMode::REVERSED:
		currentFrame = frameCount - 1;
		animDir = -1;
		break;

	case AnimationMode::LOOP:
		currentFrame = 0;
		animDir = 1;
		break;

	case AnimationMode::LOOP_PINGPONG:
		currentFrame = 0;
		animDir = 1;
		break;
	}
}

void APG::AnimatedSprite::handleNormalMode_() {
	if (currentFrame < frameCount - 1 && currentFrame > 0) {
		if (animTime > secondsPerFrame) {
			animTime -= secondsPerFrame;
			currentFrame += animDir;
		}
	} else {
		animTime = 0.0f;
	}
}

void APG::AnimatedSprite::handleLoopMode_() {
	handleNormalMode_();

	if (currentFrame == frameCount - 1) {
		currentFrame = 0;
	}
}

void APG::AnimatedSprite::handleLoopPingPongMode_() {
	handleNormalMode_();

	if (currentFrame == frameCount - 1 || currentFrame == 0) {
		animDir = (animDir == 1 ? -1 : 1);
	}
}

