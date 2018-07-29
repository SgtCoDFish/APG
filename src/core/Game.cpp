#include <cstdint>

#include "APG/core/Game.hpp"

uint32_t APG::Game::screenWidth = 1280;
uint32_t APG::Game::screenHeight = 720;

namespace APG {

Game::Game(uint32_t screenWidth, uint32_t screenHeight) {
	Game::screenWidth = screenWidth;
	Game::screenHeight = screenHeight;
}

}
