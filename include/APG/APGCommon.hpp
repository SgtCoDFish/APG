/*
 * RPGCommon.hpp
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

#ifndef RPGCOMMON_HPP_
#define RPGCOMMON_HPP_

#include <memory>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace Tmx {
class Map;
}

namespace APG {

using map_ptr = std::unique_ptr<Tmx::Map>;

struct Vertex {
	float x;
	float y;

	float c;

	float u;
	float v;
};

static const int VERTEX_SIZE = 5;

}

#endif /* RPGCOMMON_HPP_ */
