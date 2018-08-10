#ifndef APG_NO_SDL

#include <cstdint>

#include <string>
#include <stdexcept>

#include "APG/SDL.hpp"
#include "APG/GL.hpp"

#include "APG/core/SDLGame.hpp"
#include "APG/input/SDLInputManager.hpp"
#include "APG/tiled/TmxRenderer.hpp"

#include "spdlog/spdlog.h"

namespace APG {

SDLGame::SDLGame(APGContext &context) :
		Game(context.windowWidth, context.windowHeight),
		context {context},
		logger {context.logger} {
	spdlog::set_level(spdlog::level::info);
	spdlog::set_pattern("%Y-%m-%dT%H:%M:%S.%e%z [%L] [%n] %v");
	logger->info("Initialising APG with OpenGL version {}.{}", context.glContextMajor, context.glContextMinor);

	inputManager = std::make_unique<SDLInputManager>();
	audioManager = std::make_unique<SDLAudioManager>();
	fontManager = std::make_unique<PackedFontManager>(2048, 2048);

	logger->trace("Input, Audio and Font managers created.");

	window = SXXDL::make_window_ptr(
			SDL_CreateWindow(context.windowTitle.c_str(), context.windowX, context.windowY, context.windowWidth, context.windowHeight, context.sdl2WindowInitFlags));
	logger->trace("SDL window created.");

	if (window == nullptr) {
		logger->critical("Couldn't create SDL window: {}", SDL_GetError());
		throw std::runtime_error("Failed to create SDL window");
	}

#if !defined (__EMSCRIPTEN__)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, context.glContextMajor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, context.glContextMinor);
#endif

	glContext = SDL_GL_CreateContext(window.get());

	glewExperimental = GL_TRUE;
	glewInit();
	logger->trace("GL context created and GLEW initialised");

	resetGLErrors();
}

SDLGame::~SDLGame() {
	// must explicitly reset because we're about to shut SDL down;
	// if we didn't, we'd segfault because the deallocation functions
	// won't be available
	fontManager.reset();
	audioManager.reset();

	SDL_GL_DeleteContext(glContext);
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

void SDLGame::resetGLErrors() {
	// reset all errors because some are caused by glew even upon successful setup.
	auto error = glGetError();
	while (error != GL_NO_ERROR) {
		error = glGetError();
	}

	logger->trace("OpenGL errors have been reset (probably as a workaround for bugs with glew)");
}

void SDLGame::logSDLVersions(std::shared_ptr<spdlog::logger> &logger) {
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

void SDLGame::debugSDLVersion(std::shared_ptr<spdlog::logger> &logger, const char *libraryName, const SDL_version &compiledVersion,
							  const SDL_version &linkedVersion) {
	logger->trace("{} compiled with version {}.{}.{}, linked with version {}.{}.{}", libraryName,
					(uint32_t) compiledVersion.major, (uint32_t) compiledVersion.minor,
					(uint32_t) compiledVersion.patch,
					(uint32_t) linkedVersion.major, (uint32_t) linkedVersion.minor, (uint32_t) linkedVersion.patch);
}

}

#endif