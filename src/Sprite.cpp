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

#include <memory>
#include <string>
#include <sstream>

#include "APG/Sprite.hpp"
#include "APG/ErrorBase.hpp"
#include "APG/Texture.hpp"

APG::Sprite::Sprite(Texture * const texture, int32_t texX, int32_t texY, int32_t width,
		int32_t height) :
		texture(texture), texX(texX), texY(texY), width(width), height(height) {

	if (texX + width > texture->getWidth() || texY + height > texture->getHeight()) {
		std::stringstream ss;

		ss << "Invalid x/y/width/height when creating sprite.\n(x, y) = (" << texX << ", " << texY
				<< ")\n(w, h) = (" << width << ", " << height << ").\nTexture size: (w, h) = ("
				<< texture->getWidth() << ", " << texture->getHeight() << ")";

		setErrorState(ss.str().c_str());
		return;
	}

	calculateUV();
}

void APG::Sprite::calculateUV() {
	u1 = texture->getInvWidth() * texX;
	v1 = texture->getInvHeight() * texY;

	u2 = texture->getInvWidth() * (texX + width);
	v2 = texture->getInvHeight() * (texY + height);
}
