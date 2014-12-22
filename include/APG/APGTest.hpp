/*
 * APGTest.hpp
 *
 * Created on: 19 Dec 2014
 * Author: Ashley Davis (SgtCoDFish)
 */

#ifndef APGTEST_HPP_
#define APGTEST_HPP_

#include <memory>

#include "Game.hpp"
#include "SDLGame.hpp"
#include "APGCommon.hpp"
#include "SXXDL.hpp"
#include "SDLTmxRenderer.hpp"

class APGTest : public APG::SDLGame {
private:
	SXXDL::renderer_ptr renderer = SXXDL::make_renderer_ptr(nullptr);
	SDL_GLContext glcontext = nullptr;

	APG::map_ptr map;

	std::unique_ptr<APG::SDLTmxRenderer> tmxRenderer = nullptr;

public:
	APGTest() : SDLGame("APGTest",640, 480, 3, 2) {}
	virtual ~APGTest() {
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



#endif /* APGTEST_HPP_ */
