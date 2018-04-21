#ifndef APG_NO_SDL

#include <cstdint>

#include <string>

#include "APG/SDL.hpp"
#include "APG/GL.hpp"

#include "APG/core/SDLGame.hpp"
#include "APG/input/SDLInputManager.hpp"
#include "APG/tiled/TmxRenderer.hpp"

#include "easylogging++.h"

uint32_t APG::SDLGame::SDL_INIT_FLAGS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS;
uint32_t APG::SDLGame::SDL_IMAGE_INIT_FLAGS = IMG_INIT_PNG;

#if defined (__APPLE__)
uint32_t APG::SDLGame::SDL_MIXER_INIT_FLAGS = 0;
#else
uint32_t APG::SDLGame::SDL_MIXER_INIT_FLAGS = MIX_INIT_OGG;
#endif

uint32_t APG::SDLGame::SDL_WINDOW_FLAGS = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

namespace APG {

SDLGame::SDLGame(const std::string &windowTitle, uint32_t windowWidth, uint32_t windowHeight, uint32_t glContextMajor,
				 uint32_t glContextMinor, uint32_t windowX, uint32_t windowY) :
		APG::Game(windowWidth, windowHeight) {
	const auto logger = el::Loggers::getLogger("APG");
	logger->info("Initialising APG with OpenGL version %v.%v.", glContextMajor, glContextMinor);

	SDLGame::initialiseSDL(logger);

	inputManager = std::make_unique<SDLInputManager>();
	audioManager = std::make_unique<SDLAudioManager>();
	fontManager = std::make_unique<PackedFontManager>(2048, 2048);

	window = SXXDL::make_window_ptr(
			SDL_CreateWindow(windowTitle.c_str(), windowX, windowY, windowWidth, windowHeight, SDL_WINDOW_FLAGS));

	if (window == nullptr) {
		logger->fatal("Couldn't create SDL window: %v", SDL_GetError());
	}

#if !defined (__EMSCRIPTEN__)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glContextMajor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glContextMinor);
#endif

	glContext = SDL_GL_CreateContext(window.get());

	glewExperimental = GL_TRUE;
	glewInit();

	resetGLErrors();
}

SDLGame::~SDLGame() {
	// must explicitly reset because we're about to shut SDL down;
	// if we didn't, we'd segfault because the deallocation functions
	// won't be available
	fontManager.reset();
	audioManager.reset();

	SDL_GL_DeleteContext(glContext);
	SDLGame::shutdownSDL();
}

void SDLGame::handleEvent(SDL_Event &event) {
	if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP || event.type == SDL_MOUSEBUTTONUP ||
		event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEMOTION) {
		inputManager->handleInputEvent(event);
	} else if (event.type == SDL_QUIT) {
		quit();
	}
}

bool SDLGame::update(float deltaTime) {
	if (shouldQuit) {
		return true;
	}

	inputManager->update(deltaTime);

	while (SDL_PollEvent(&eventCache)) {
		handleEvent(eventCache);
	}

	render(deltaTime);

	return false;
}

void SDLGame::quit() {
	shouldQuit = true;
}

void SDLGame::initialiseSDL(el::Logger *logger) {
	if (logger == nullptr) {
		logger = el::Loggers::getLogger("APG");
	}

	if (SDL_Init(SDL_INIT_FLAGS) < 0) {
		logger->fatal("Couldn't initialise SDL2: %v", SDL_GetError());
	} else {
		logger->verbose(1, "Successfully initialised SDL2.");
	}

	if ((IMG_Init(SDL_IMAGE_INIT_FLAGS) & SDL_IMAGE_INIT_FLAGS) != SDL_IMAGE_INIT_FLAGS) {
		logger->fatal("Couldn't initialise SDL2_image: %v", IMG_GetError());
	} else {
		logger->verbose(1, "Successfully initialised SDL2_image.");
	}

	if (TTF_Init() == -1) {
		logger->fatal("Couldn't initialise SDL2_ttf: %v", TTF_GetError());
	} else {
		logger->verbose(1, "Successfully initialised SDL2_ttf.");
	}

	if ((Mix_Init(SDL_MIXER_INIT_FLAGS) & SDL_MIXER_INIT_FLAGS) != SDL_MIXER_INIT_FLAGS) {
		logger->fatal("Couldn't initialise SDL2_mixer: %v", Mix_GetError());
	} else {
		logger->verbose(1, "Successfully initialised SDL2_mixer.");
	}

	if (SDLNet_Init() == -1) {
		logger->fatal("Couldn't initialise SDL2_net: %v", SDLNet_GetError());
	} else {
		logger->verbose(1, "Successfully initialised SDL2_net");
	}

	SDLGame::logSDLVersions();
}

void SDLGame::shutdownSDL() {
	SDLNet_Quit();
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void SDLGame::resetGLErrors() {
	// reset all errors because some are caused by glew even upon successful setup.
	auto error = glGetError();
	while (error != GL_NO_ERROR) {
		error = glGetError();
	}

	el::Loggers::getLogger("APG")->verbose(9,
										   "OpenGL errors have been reset (probably as a workaround for bugs with glew)");
}

void SDLGame::logSDLVersions() {
	const auto logger = el::Loggers::getLogger("APG");

	SDL_version compiledVersion, sdlLinkedVersion;

	SDL_VERSION(&compiledVersion);
	SDL_GetVersion(&sdlLinkedVersion);

	SDLGame::debugSDLVersion(logger, "SDL2", compiledVersion, sdlLinkedVersion);

	SDL_IMAGE_VERSION(&compiledVersion);
	const auto imgVersion = IMG_Linked_Version();
	SDLGame::debugSDLVersion(logger, "SDL2_image", compiledVersion, *imgVersion);

	SDL_TTF_VERSION(&compiledVersion);
	const auto ttfVersion = TTF_Linked_Version();
	SDLGame::debugSDLVersion(logger, "SDL2_ttf", compiledVersion, *ttfVersion);

	SDL_MIXER_VERSION(&compiledVersion);
#if !defined (__EMSCRIPTEN__)
	const auto mixVersion = Mix_Linked_Version();
	SDLGame::debugSDLVersion(logger, "SDL2_mixer", compiledVersion, *mixVersion);
#endif

	SDL_NET_VERSION(&compiledVersion);
	const auto netVersion = SDLNet_Linked_Version();
	SDLGame::debugSDLVersion(logger, "SDL2_net", compiledVersion, *netVersion);
}

void SDLGame::debugSDLVersion(el::Logger *const logger, const char *libraryName, const SDL_version &compiledVersion,
							  const SDL_version &linkedVersion) {
	logger->verbose(8, "%v compiled with version %v.%v.%v, linked with version %v.%v.%v", libraryName,
					(uint32_t) compiledVersion.major, (uint32_t) compiledVersion.minor,
					(uint32_t) compiledVersion.patch,
					(uint32_t) linkedVersion.major, (uint32_t) linkedVersion.minor, (uint32_t) linkedVersion.patch);
}

}

#endif
