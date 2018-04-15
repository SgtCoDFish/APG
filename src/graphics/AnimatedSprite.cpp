#ifndef APG_NO_SDL
#ifndef APG_NO_GL

#include <cstdint>

#include <initializer_list>
#include <vector>
#include <utility>
#include <iostream>

#include "Tmx.h"

#include "APG/graphics/AnimatedSprite.hpp"
#include "APG/graphics/Texture.hpp"
#include "APG/internal/Assert.hpp"

namespace APG {

AnimatedSprite::AnimatedSprite(float frameDuration, Sprite *firstFrame, AnimationMode animationMode) :
		secondsPerFrame{frameDuration} {

	initializeFromSpriteFrame(firstFrame);
	addFrame(firstFrame);
	setAnimationMode(animationMode);
}

AnimatedSprite::AnimatedSprite(float frameDuration, std::initializer_list<SpriteBase *> &sprites,
									AnimationMode animationMode) :
		secondsPerFrame{frameDuration} {
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

AnimatedSprite::AnimatedSprite(float frameDuration, std::vector<Sprite> &sprites, AnimationMode animationMode) :
		secondsPerFrame{frameDuration} {
	REQUIRE(!sprites.empty(), "Can't initialise an animated sprite with an empty sprite list.");

	initializeFromSpriteFrame(&(sprites.front()));

	for (auto &sprite : sprites) {
		addFrame(&sprite);
	}

	setAnimationMode(animationMode);
}

AnimatedSprite::AnimatedSprite(float frameDuration, std::vector<SpriteBase *> &sprites,
									AnimationMode animationMode) :
		secondsPerFrame{frameDuration} {
	REQUIRE(!sprites.empty(), "Can't initialise an animated sprite with an empty sprite list.");
	initializeFromSpriteFrame(sprites.front());

	for (auto sprite : sprites) {
		addFrame(sprite);
	}

	setAnimationMode(animationMode);
}

void AnimatedSprite::addFrame(SpriteBase *frame) {
	REQUIRE(frame->getWidth() == this->width && frame->getHeight() == this->height,
			"Frame dimensions must match existing frames for animated sprites.");
	REQUIRE(frame->getTexture() == this->texture, "Cannot have an animation spanning two different textures.");

	frames.emplace_back(frame);

	frameCount++;
}

void AnimatedSprite::update(float deltaTime) {
	animationTime += deltaTime;

	switch (animationMode) {
		case AnimationMode::NORMAL:
			handleNormalMode_();
			break;

		case AnimationMode::REVERSED:
			handleReversedMode_();
			break;

		case AnimationMode::LOOP:
			handleLoopMode_();
			break;

		case AnimationMode::LOOP_PINGPONG:
			handleLoopPingPongMode_();
			break;
	}
}

SpriteBase *AnimatedSprite::getFrame(uint32_t frameNumber) const {
	REQUIRE((int32_t) frameNumber >= 0 && (int32_t) frameNumber <= frameCount,
			"Invalid frame number passed to getFrame.");
	return frames[frameNumber];
}

SpriteBase *AnimatedSprite::getCurrentFrame() const {
	return frames[currentFrame];
}

AnimatedSprite &AnimatedSprite::setAnimationMode(AnimationMode mode) {
	this->animationMode = mode;

	switch (this->animationMode) {
		case AnimationMode::NORMAL:
			currentFrame = 0;
			break;

		case AnimationMode::REVERSED:
			currentFrame = frameCount - 1;
			animationDirection = -1;
			break;

		case AnimationMode::LOOP:
			currentFrame = 0;
			animationDirection = 1;
			break;

		case AnimationMode::LOOP_PINGPONG:
			currentFrame = 0;
			animationDirection = 1;
			break;
	}

	return *this;
}

void AnimatedSprite::progress_() {
	if (animationTime > secondsPerFrame) {
		animationTime -= secondsPerFrame;
		currentFrame += animationDirection;
	}
}

void AnimatedSprite::handleNormalMode_() {
	progress_();

	if (currentFrame >= frameCount) {
		currentFrame = frameCount - 1;
	}
}

void AnimatedSprite::handleLoopMode_() {
	progress_();

	if (currentFrame >= frameCount) {
		currentFrame = 0;
	}
}

void AnimatedSprite::handleLoopPingPongMode_() {
	progress_();

	if (currentFrame >= frameCount) {
		animationDirection = -1;
		currentFrame = frameCount - 1;
	} else if (currentFrame <= 0) {
		animationDirection = 1;
		currentFrame = 0;
	}
}

void AnimatedSprite::handleReversedMode_() {
	progress_();

	if (currentFrame <= 0) {
		currentFrame = 0;
	}
}

void AnimatedSprite::initializeFromSpriteFrame(const SpriteBase *sprite) {
	this->width = sprite->getWidth();
	this->height = sprite->getHeight();
	this->texture = sprite->getTexture();
}

std::vector<Sprite> AnimatedSprite::splitTexture(Texture *texture, int32_t tileWidth, int32_t tileHeight,
														   int32_t xStart, int32_t yStart, int32_t rawFrameCount,
														   int32_t xSeparation) {
	const auto textureWidth = texture->getWidth();
	const auto textureHeight = texture->getHeight();

	REQUIRE(textureWidth >= (xStart + tileWidth + xSeparation) && textureHeight >= (yStart + tileHeight),
			"Texture to split must be bigger than the tiles being extracted and tiles must fit inside the texture..");

	size_t frameCount;

	if (rawFrameCount < 0) {
		frameCount = static_cast<size_t>((texture->getWidth() - xStart) / (tileWidth + xSeparation));
	} else {
		frameCount = static_cast<size_t>(rawFrameCount);
	}

	REQUIRE(frameCount >= 1, "Cannot have 0 frames in a split texture.");

	std::vector<Sprite> loadedFrames;
	loadedFrames.reserve(frameCount);

	for (uint32_t i = 0u; i < (uint32_t) frameCount; ++i) {
		const auto x = xStart + i * (tileWidth + xSeparation);
		const auto y = yStart;

		loadedFrames.emplace_back(texture, x, y, tileWidth, tileHeight);
	}

	return loadedFrames;
}

}

#endif
#endif

