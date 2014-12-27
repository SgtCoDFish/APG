/*
 * APGTest.hpp
 *
 * Created on: 19 Dec 2014
 * Author: Ashley Davis (SgtCoDFish)
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
	SDL_GLContext glcontext = nullptr;

	APG::map_ptr map;

	std::unique_ptr<APG::SDLTmxRenderer> sdlTmxRenderer = nullptr;

public:
	APGSDLRenderTest() : SDLGame("APGSDLRenderTest", 1280, 720, 3, 2) {}
	virtual ~APGSDLRenderTest() {
		if(glcontext != nullptr) {
			SDL_GL_DeleteContext(glcontext);
		}
	}

	bool init() override;
	void render(float deltaTime) override;

	const Tmx::Map *getMap() const {
		return map.get();
	}
};

#endif /* APGSDLRENDERTEST_HPP_ */
