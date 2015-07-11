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

#include <cstdint>

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "APG/APGeasylogging.hpp"

#include "APG/SDLGame.hpp"
#include "APG/SDLInputManager.hpp"
#include "APG/TmxRenderer.hpp"

uint32_t APG::SDLGame::SDL_INIT_FLAGS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS;
uint32_t APG::SDLGame::SDL_IMAGE_INIT_FLAGS = IMG_INIT_PNG;
uint32_t APG::SDLGame::SDL_MIXER_INIT_FLAGS = MIX_INIT_OGG;
uint32_t APG::SDLGame::SDL_WINDOW_FLAGS = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

APG::SDLGame::SDLGame(const std::string &windowTitle, uint32_t windowWidth, uint32_t windowHeight,
        uint32_t glContextMajor, uint32_t glContextMinor, uint32_t windowX, uint32_t windowY) :
		APG::Game(windowWidth, windowHeight) {
	const auto logger = el::Loggers::getLogger("APG");
	logger->info("Initialising APG with OpenGL version %v.%v.", glContextMajor, glContextMinor);

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

	if(TTF_Init() == -1) {
		logger->fatal("Couldn't initialise SDL2_ttf: %v", TTF_GetError());
	} else {
		logger->verbose(1, "Successfully initialised SDL2_ttf.");
	}

	if ((Mix_Init(SDL_MIXER_INIT_FLAGS) & SDL_MIXER_INIT_FLAGS) != SDL_MIXER_INIT_FLAGS) {
		logger->fatal("Couldn't initialise SDL2_mixer: %v", Mix_GetError());
	} else {
		logger->verbose(1, "Successfully initialised SDL2_mixer.");
	}

	logSDLVersions();

	inputManager = std::make_unique<SDLInputManager>();
	audioManager = std::make_unique<SDLAudioManager>();
	fontManager = std::make_unique<SDLFontManager>();

	window = SXXDL::make_window_ptr(
	        SDL_CreateWindow(windowTitle.c_str(), windowX, windowY, windowWidth, windowHeight, SDL_WINDOW_FLAGS));

	if (window == nullptr) {
		logger->fatal("Couldn't create SDL window: %v", SDL_GetError());
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glContextMajor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glContextMinor);

	glContext = SDL_GL_CreateContext(window.get());

	glewExperimental = GL_TRUE;
	glewInit();

	// reset all errors because some are caused by glew even upon successful setup.
	auto error = glGetError();
	while (error != GL_NO_ERROR) {
		error = glGetError();
	}

	logger->verbose(9, "OpenGL errors have been reset (probably as a workaround for any bugs with glew)");
}

APG::SDLGame::~SDLGame() {
	fontManager = nullptr;
	audioManager = nullptr;
	inputManager = nullptr;

	SDL_GL_DeleteContext(glContext);
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void APG::SDLGame::handleEvent(SDL_Event &event) {
	if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
		inputManager->handleInputEvent(event);
	} else if (event.type == SDL_QUIT) {
		quit();
	}
}

bool APG::SDLGame::update(float deltaTime) {
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

void APG::SDLGame::quit() {
	shouldQuit = true;
}

void APG::SDLGame::logSDLVersions() const {
	const auto logger = el::Loggers::getLogger("APG");

	SDL_version compiledVersion, sdlLinkedVersion;

	SDL_VERSION(&compiledVersion);
	SDL_GetVersion(&sdlLinkedVersion);

	debugSDLVersion(logger, "SDL2", compiledVersion, sdlLinkedVersion);

	SDL_IMAGE_VERSION(&compiledVersion);
	const auto imgVersion = IMG_Linked_Version();
	debugSDLVersion(logger, "SDL2_image", compiledVersion, *imgVersion);

	SDL_TTF_VERSION(&compiledVersion);
	const auto ttfVersion = TTF_Linked_Version();
	debugSDLVersion(logger, "SDL2_ttf", compiledVersion, *ttfVersion);

	SDL_MIXER_VERSION(&compiledVersion);
	const auto mixVersion = Mix_Linked_Version();
	debugSDLVersion(logger, "SDL2_mixer", compiledVersion, *mixVersion);
}

void APG::SDLGame::debugSDLVersion(el::Logger * const logger, const char *libraryName,
        const SDL_version &compiledVersion, const SDL_version &linkedVersion) const {
	logger->verbose(9, "%v compiled with version %v.%v.%v, linked with version %v.%v.%v", libraryName,
	        (uint32_t) compiledVersion.major, (uint32_t) compiledVersion.minor, (uint32_t) compiledVersion.patch,
	        (uint32_t) linkedVersion.major, (uint32_t) linkedVersion.minor, (uint32_t) linkedVersion.patch);
}
