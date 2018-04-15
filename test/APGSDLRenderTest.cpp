#include <cstdlib>

#include <memory>
#include <chrono>
#include <vector>
#include <numeric>
#include <utility>

#include "APG/SDL.hpp"
#include "APG/GL.hpp"

#include "Tmx.h"

#include "APG/SXXDL.hpp"
#include "APG/core/Game.hpp"
#include "APG/tiled/SDLTmxRenderer.hpp"

#include "test/APGSDLRenderTest.hpp"

#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

#if defined (__EMSCRIPTEN__)
#include <emscripten.h>
#endif

const std::string ASSET_PREFIX = "assets/";

bool APGSDLRenderTest::init() {
	const auto logger = el::Loggers::getLogger("APG");
	renderer = SXXDL::make_renderer_ptr(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED));

	if (renderer == nullptr) {
		logger->fatal("Couldn't create SDL_Renderer: %v", SDL_GetError());
		return false;
	}

	SDL_SetRenderDrawColor(renderer.get(), 0x50, 0xAC, 0x3D, 0xFF);

	auto mapOne = std::make_unique<Tmx::Map>();
	mapOne->ParseFile(ASSET_PREFIX + "world1.tmx");

	if (mapOne->HasError()) {
		logger->fatal("Error loading tmx map: %v", mapOne->GetErrorText());
		return false;
	}

	auto mapTwo = std::make_unique<Tmx::Map>();
	mapTwo->ParseFile(ASSET_PREFIX + "sample_indoor.tmx");

	if (mapTwo->HasError()) {
		logger->fatal("Error loading tmx map: %v", mapTwo->GetErrorText());
		return false;
	}

	rendererOne = std::make_unique<APG::SDLTmxRenderer>(std::move(mapOne), renderer);
	rendererTwo = std::make_unique<APG::SDLTmxRenderer>(std::move(mapTwo), renderer);

	currentRenderer = rendererOne.get();

	return true;
}

void APGSDLRenderTest::render(float deltaTime) {
	SDL_RenderClear(renderer.get());

	if (inputManager->isKeyJustPressed(SDL_SCANCODE_SPACE)) {
		if (currentRenderer == rendererOne.get()) {
			currentRenderer = rendererTwo.get();
		} else {
			currentRenderer = rendererOne.get();
		}
	}

	currentRenderer->renderAll(deltaTime);

	SDL_RenderPresent(renderer.get());
}

struct loop_arg {
	APGSDLRenderTest *rpg;
	std::chrono::time_point<std::chrono::high_resolution_clock> timepoint;
	std::vector<float> timesTaken;
	el::Logger *logger;
	bool done;
};

void loop(void *v_arg) {
	loop_arg * arg = static_cast<loop_arg *>(v_arg);

	auto timeNow = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - arg->timepoint).count() / 1000.0f;

	arg->timepoint = timeNow;
	arg->timesTaken.push_back(deltaTime);

	arg->done = arg->rpg->update(deltaTime);

	if (arg->timesTaken.size() >= 1000) {
		const float sum = std::accumulate(arg->timesTaken.begin(), arg->timesTaken.end(), 0.0f);
		const float fps = 1 / (sum / arg->timesTaken.size());

		arg->logger->info("FPS: ", fps);

		arg->timesTaken.clear();
	}
}

int main(int argc, char *argv[]) {
	START_EASYLOGGINGPP(argc, argv);

	const std::string windowTitle("APG SDLTmxRenderer Example");
	const uint32_t windowWidth = 1280;
	const uint32_t windowHeight = 720;

	const auto logger = el::Loggers::getLogger("APG");

	auto rpg = std::make_unique<APGSDLRenderTest>(windowTitle, windowWidth, windowHeight);

	if (!rpg->init()) {
		return EXIT_FAILURE;
	}

	loop_arg arg;
	arg.rpg = rpg.get();
	arg.timepoint = std::chrono::high_resolution_clock::now();
	arg.done = false;
	arg.logger =  logger;

#if defined(__EMSCRIPTEN__)
	emscripten_set_main_loop_arg(loop, &arg, 0, 1);
#else
	while (!arg.done) {
		loop(&arg);
	}
#endif

	return EXIT_SUCCESS;
}

