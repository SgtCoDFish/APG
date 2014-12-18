/*
 * ArrPeeGee.hpp
 * Copyright (C) 2014 Ashley Davis (SgtCoDFish)
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

#ifndef ARRPEEGEE_HPP_
#define ARRPEEGEE_HPP_

#include <memory>

#include "ErrorBase.hpp"
#include "APGCommon.hpp"
#include "SXXDL.hpp"
#include "SDLTmxRenderer.hpp"

namespace Tmx {
	class Map;
}

namespace APG {
class Game : public ErrorBase {
private:
	SXXDL::window_ptr window = SXXDL::make_window_ptr(nullptr);
	SXXDL::renderer_ptr renderer = SXXDL::make_renderer_ptr(nullptr);

	map_ptr map;

	SDL_Event eventStore;

	std::unique_ptr<SDLTmxRenderer> tmxRenderer = nullptr;

public:
	Game();
	~Game();

	bool init();
	bool update(float deltaTime);
	void render(float deltaTime);

	const Tmx::Map *getMap() const {
		return map.get();
	}
};

}

#endif /* ARRPEEGEE_HPP_ */
