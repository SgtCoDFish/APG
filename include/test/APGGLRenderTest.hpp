/*
 * Copyright (c) 2014, 2015 Ashley Davis (SgtCoDFish)
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

#ifndef APGGLRENDERTEST_HPP_
#define APGGLRENDERTEST_HPP_

#include <string>
#include <memory>
#include <utility>

#include <SDL2/SDL.h>

#include "APG/Game.hpp"
#include "APG/SDLGame.hpp"
#include "APG/APGCommon.hpp"
#include "APG/SXXDL.hpp"
#include "APG/ShaderProgram.hpp"
#include "APG/GLTmxRenderer.hpp"
#include "APG/Buffer.hpp"
#include "APG/VAO.hpp"
#include "APG/Texture.hpp"
#include "APG/SpriteBatch.hpp"
#include "APG/Sprite.hpp"

#include "tmxparser/TmxMap.h"

namespace APG {

class APGGLRenderTest final : public APG::SDLGame {
private:
	static const char *vertexShaderFilename;
	static const char *fragmentShaderFilename;

	std::unique_ptr<Tmx::Map> map1;
	std::unique_ptr<Tmx::Map> map2;

	std::unique_ptr<ShaderProgram> shaderProgram;

	std::unique_ptr<SpriteBatch> spriteBatch;

	std::unique_ptr<GLTmxRenderer> rendererOne;
	std::unique_ptr<GLTmxRenderer> rendererTwo;
	GLTmxRenderer *currentRenderer = nullptr;

	std::unique_ptr<Texture> playerTexture;
	std::vector<Sprite> playerFrames;
	std::unique_ptr<AnimatedSprite> playerAnimation;

public:
	explicit APGGLRenderTest(const std::string &windowTitle, uint32_t windowWidth, uint32_t windowHeight,
	        uint32_t glContextMajor = 3, uint32_t glContextMinor = 2, uint32_t windowX = SDL_WINDOWPOS_CENTERED, uint32_t windowY = SDL_WINDOWPOS_CENTERED) :
			SDLGame(windowTitle, windowWidth, windowHeight, glContextMajor, glContextMinor, windowX, windowY) {
	}

	virtual ~APGGLRenderTest() = default;

	bool init() override;
	void render(float deltaTime) override;

	const Tmx::Map *getMap() const {
		return map1.get();
	}
};

}

#endif /* APGGLRENDERTEST_HPP_ */
