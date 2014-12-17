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
 *
 */

#ifndef ARRPEEGEE_HPP_
#define ARRPEEGEE_HPP_

#include <memory>

struct SDL_Window;
struct SDL_Surface;

namespace Tmx {
	class Map;
}

namespace RPG {

class RPG {
private:
	bool initFailure = false;

	std::unique_ptr<SDL_Window, void(*)(SDL_Window *)> window = std::unique_ptr<SDL_Window, void(*)(SDL_Window *)>(nullptr, nullptr);
	std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer *)> renderer = std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer *)>(nullptr, nullptr);

	std::unique_ptr<Tmx::Map> map;

	SDL_Event eventStore;

public:
	RPG();
	~RPG();

	bool init();
	bool update(float deltaTime);
	void render(float deltaTime);

	const Tmx::Map *getMap() const {
		return map.get();
	}
};

}

#endif /* ARRPEEGEE_HPP_ */
