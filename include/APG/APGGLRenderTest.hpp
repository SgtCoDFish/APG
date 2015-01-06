/*
 * APGGLRenderTest.hpp
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

#ifndef APGGLRENDERTEST_HPP_
#define APGGLRENDERTEST_HPP_

#include <string>
#include <memory>
#include <utility>

#include "Game.hpp"
#include "SDLGame.hpp"
#include "APGCommon.hpp"
#include "SXXDL.hpp"
#include "ShaderProgram.hpp"
#include "GLTmxRenderer.hpp"
#include "Buffer.hpp"
#include "VAO.hpp"
#include "Texture.hpp"
#include "SpriteBatch.hpp"
#include "Sprite.hpp"

#include "TmxMap.h"

namespace APG {

class APGGLRenderTest : public APG::SDLGame {
private:
	static const char *vertexShaderFilename;
	static const char *fragmentShaderFilename;
	std::unique_ptr<Tmx::Map> map;

	std::unique_ptr<ShaderProgram> shaderProgram;

	std::unique_ptr<SpriteBatch> spriteBatch;

	std::unique_ptr<GLTmxRenderer> renderer;

	std::unique_ptr<Sprite> sprite;
public:
	APGGLRenderTest(const std::string &title, uint32_t screenWidth, uint32_t screenHeight) :
			SDLGame(title, screenWidth, screenHeight, 3, 2) {
	}

	virtual ~APGGLRenderTest() = default;

	bool init() override;
	void render(float deltaTime) override;

	const Tmx::Map *getMap() const {
		return map.get();
	}
};

}

#endif /* APGGLRENDERTEST_HPP_ */
