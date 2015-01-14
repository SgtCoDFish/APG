/*
 * ShaderProgram.hpp
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

#include "TmxTile.h"

#include "AnimatedSprite.hpp"

int APG::AnimatedSprite::getU() const {
	return Sprite::getU();
}

int APG::AnimatedSprite::getV() const {
	return Sprite::getV();
}

int APG::AnimatedSprite::getU2() const {
	return Sprite::getU2();
}

int APG::AnimatedSprite::getV2() const {
	return Sprite::getV2();
}
