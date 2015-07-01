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
#include <iostream>

#include "tmxparser/TmxTile.h"

#include "APG/AnimatedSprite.hpp"

#include "APG/internal/Assert.hpp"

APG::AnimatedSprite::AnimatedSprite(float frameDuration, Sprite * firstFrame, AnimationMode animationMode) :
		secondsPerFrame { frameDuration } {

	initializeFromSpriteFrame(firstFrame);
	addFrame(firstFrame);
	setAnimationMode(animationMode);
}

APG::AnimatedSprite::AnimatedSprite(float frameDuration, std::initializer_list<SpriteBase *> &sprites,
        AnimationMode animationMode) :
		secondsPerFrame { frameDuration } {
	REQUIRE(sprites.size() > 0, "Can't initialise an animated sprite with an empty sprite list.");

	bool first = true;
	for (auto sprite : sprites) {
		if (first) {
			initializeFromSpriteFrame(sprite);
			first = false;
		}

		addFrame(sprite);
	}

	setAnimationMode(animationMode);
}

APG::AnimatedSprite::AnimatedSprite(float frameDuration, std::vector<SpriteBase *> &sprites,
        AnimationMode animationMode) :
		secondsPerFrame { frameDuration } {
	REQUIRE(sprites.size() > 0, "Can't initialise an animated sprite with an empty sprite list.");
	initializeFromSpriteFrame(sprites.front());

	for (auto sprite : sprites) {
		addFrame(sprite);
	}

	setAnimationMode(animationMode);
}

void APG::AnimatedSprite::addFrame(SpriteBase * frame) {
	REQUIRE(frame->getWidth() == this->width && frame->getHeight() == this->height,
	        "Frame dimensions must match existing frames for animated sprites.");
	REQUIRE(frame->getTexture() == this->texture, "Cannot have an animation spanning two different textures.");

	frames.emplace_back(frame);

	frameCount++;
//	std::cout << "Added new animation frame: " << frames.back()->getHash() << ", count is now " << frameCount << ", frame has ID " << frame->getHash() << "\n";
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
		REQUIRE(false, "Unsupported animation type in update()");
		break;
	}
}

APG::SpriteBase *APG::AnimatedSprite::getFrame(uint32_t frameNumber) const {
	REQUIRE(frameNumber >= 0 && frameNumber <= frameCount, "Invalid frame number passed to getFrame.");
	return frames[frameNumber];
}

APG::SpriteBase *APG::AnimatedSprite::getCurrentFrame() const {
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
	if (currentFrame < frameCount) {
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

	if (currentFrame == frameCount) {
		currentFrame = 0;
	}
}

void APG::AnimatedSprite::handleLoopPingPongMode_() {
	handleNormalMode_();

	if (currentFrame == frameCount - 1) {
		animDir = -1;
	} else if (currentFrame == 0) {
		animDir = 1;
	}
}

void APG::AnimatedSprite::initializeFromSpriteFrame(SpriteBase * sprite) {
	this->width = sprite->getWidth();
	this->height = sprite->getHeight();
	this->u = sprite->getU();
	this->v = sprite->getV();
	this->u2 = sprite->getU2();
	this->v2 = sprite->getV2();
	this->texture = sprite->getTexture();
}

