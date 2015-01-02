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

#ifndef SPRITE_HPP_
#define SPRITE_HPP_

#include "Buffer.hpp"

namespace APG {

class Sprite : public FloatBuffer {
private:
	std::unique_ptr<uint32_t[]> indices = std::unique_ptr<uint32_t[]>(nullptr);
	std::unique_ptr<UInt32Buffer> ebo = std::unique_ptr<UInt32Buffer>(nullptr);

public:
	Sprite(DrawType vertexDrawHint = DrawType::DYNAMIC_DRAW) :
			FloatBuffer(BufferType::ARRAY, vertexDrawHint) {
	}
};

}

#endif /* SPRITE_HPP_ */
