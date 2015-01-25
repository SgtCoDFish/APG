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

#ifndef SPRITE_HPP_
#define SPRITE_HPP_

#include <cstdint>

#include <memory>

#include "APG/Buffer.hpp"
#include "APG/ErrorBase.hpp"

namespace APG {

class Texture;

class Sprite : public ErrorBase {
protected:
	Texture * texture = nullptr;

	uint32_t texX = 0, texY = 0;

	uint32_t width = 0, height = 0;

	float u1 = 0.0f, v1 = 0.0f;
	float u2 = 0.0f, v2 = 0.0f;

	void calculateUV();

public:
	Sprite(Texture * const texture);
	Sprite(Texture * const texture, uint32_t texX, uint32_t texY, uint32_t width, uint32_t height);

	Texture * getTexture() const {
		return texture;
	}

	inline int32_t getWidth() const {
		return width;
	}

	inline int32_t getHeight() const {
		return height;
	}

	inline float getU() const {
		return u1;
	}

	inline float getV() const {
		return v1;
	}

	inline float getU2() const {
		return u2;
	}

	inline float getV2() const {
		return v2;
	}

	Sprite(Sprite &other) = delete;
	Sprite(const Sprite &other) = delete;

	Sprite &operator=(Sprite &other) = delete;
	Sprite &operator=(const Sprite &other) = delete;
};

}

#endif /* SPRITE_HPP_ */
