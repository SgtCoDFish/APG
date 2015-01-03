/*
 * Sprite.hpp
 * Copyright (C) 2014, 2015 Ashley Davis (SgtCoDFish)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <cstdint>

#include <memory>
#include <string>
#include <sstream>

#include "Sprite.hpp"
#include "ErrorBase.hpp"
#include "Texture.hpp"

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
