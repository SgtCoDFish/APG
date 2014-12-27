/*
 * APGTest.hpp
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

#ifndef APGSDLRENDERTEST_HPP_
#define APGSDLRENDERTEST_HPP_

#include <memory>

#include "Game.hpp"
#include "SDLGame.hpp"
#include "APGCommon.hpp"
#include "SXXDL.hpp"
#include "SDLTmxRenderer.hpp"

class APGSDLRenderTest : public APG::SDLGame {
private:
	SXXDL::renderer_ptr renderer = SXXDL::make_renderer_ptr(nullptr);

	APG::map_ptr map;

	std::unique_ptr<APG::SDLTmxRenderer> sdlTmxRenderer = nullptr;

public:
	APGSDLRenderTest() : SDLGame("APGSDLRenderTest", 1280, 720, 3, 2) {}
	virtual ~APGSDLRenderTest() = default;

	bool init() override;
	void render(float deltaTime) override;

	const Tmx::Map *getMap() const {
		return map.get();
	}
};

#endif /* APGSDLRENDERTEST_HPP_ */
