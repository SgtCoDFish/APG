/*
 * IndexBufferObject.hpp
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

#ifndef INDEXBUFFEROBJECT_HPP_
#define INDEXBUFFEROBJECT_HPP_

#include <cstdint>

#include "Buffer.hpp"

namespace APG {

class IndexBufferObject {
private:
	const DrawType drawType;

	UInt32Buffer buffer;

public:
	explicit IndexBufferObject(bool isStatic = false);

	inline const DrawType getDrawType() const {
		return drawType;
	}

	inline const UInt32Buffer &getBuffer() const {
		return buffer;
	}
};

}

#endif /* INDEXBUFFEROBJECT_HPP_ */
