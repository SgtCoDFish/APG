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
#include "APGCommon.hpp"
#include "SXXDL.hpp"
#include "SDLTmxRenderer.hpp"

class APGTest : public APG::Game {
private:
	SXXDL::window_ptr window = SXXDL::make_window_ptr(nullptr);
	SXXDL::renderer_ptr renderer = SXXDL::make_renderer_ptr(nullptr);

	APG::map_ptr map;

	SDL_Event eventStore;

	std::unique_ptr<APG::SDLTmxRenderer> tmxRenderer = nullptr;

public:
	APGTest() : Game() {}
	~APGTest() {}

	bool init() override;
	bool update(float deltaTime) override;
	void render(float deltaTime) override;

	const Tmx::Map *getMap() const {
		return map.get();
	}
};



#endif /* APGTEST_HPP_ */
