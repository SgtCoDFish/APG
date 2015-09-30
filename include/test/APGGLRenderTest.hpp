/*
 * Copyright (c) 2014, 2015 See AUTHORS file.
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

#ifndef INCLUDE_TEST_APGGLRENDERTEST_HPP_
#define INCLUDE_TEST_APGGLRENDERTEST_HPP_

#include <string>
#include <memory>
#include <utility>

#include <SDL2/SDL.h>

#include "tmxparser/TmxMap.h"

#include "APG/SXXDL.hpp"
#include "APG/core/Game.hpp"
#include "APG/core/SDLGame.hpp"
#include "APG/core/APGCommon.hpp"
#include "APG/graphics/ShaderProgram.hpp"
#include "APG/graphics/Buffer.hpp"
#include "APG/graphics/VAO.hpp"
#include "APG/graphics/Texture.hpp"
#include "APG/graphics/SpriteBatch.hpp"
#include "APG/graphics/Sprite.hpp"
#include "APG/graphics/Camera.hpp"
#include "APG/tiled/GLTmxRenderer.hpp"

namespace APG {

class APGGLRenderTest final : public APG::SDLGame {
public:
	explicit APGGLRenderTest(const std::string &windowTitle, uint32_t windowWidth, uint32_t windowHeight,
	        uint32_t glContextMajor = 3, uint32_t glContextMinor = 2, uint32_t windowX = SDL_WINDOWPOS_CENTERED,
	        uint32_t windowY = SDL_WINDOWPOS_CENTERED) :
			        SDLGame(windowTitle, windowWidth, windowHeight, glContextMajor, glContextMinor, windowX, windowY) {
	}

	virtual ~APGGLRenderTest() = default;

	bool init() override;
	void render(float deltaTime) override;

private:
	static const char *vertexShaderFilename;
	static const char *fragmentShaderFilename;

	std::unique_ptr<ShaderProgram> shaderProgram;

	std::unique_ptr<Camera> camera;
	std::unique_ptr<SpriteBatch> spriteBatch;

	std::unique_ptr<GLTmxRenderer> rendererOne;
	std::unique_ptr<GLTmxRenderer> rendererTwo;
	GLTmxRenderer *currentRenderer = nullptr;

	std::unique_ptr<Texture> playerTexture;
	std::vector<Sprite> playerFrames;
	std::unique_ptr<AnimatedSprite> playerAnimation;

	std::unique_ptr<Texture> miniTexture;
	std::unique_ptr<Sprite> miniPlayer;

	SpriteBase * currentPlayer = nullptr;

	FontManager::font_handle font = -1;

	SpriteBase *fontSprite = nullptr;
};

}

#endif /* APGGLRENDERTEST_HPP_ */
