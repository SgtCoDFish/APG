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

#ifndef INCLUDE_APG_CORE_SDLGAME_HPP_
#define INCLUDE_APG_CORE_SDLGAME_HPP_

#include <cstdint>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>


#include "APG/SXXDL.hpp"
#include "APG/core/Game.hpp"
#include "APG/core/APGeasylogging.hpp"
#include "APG/input/SDLInputManager.hpp"
#include "APG/audio/SDLAudioManager.hpp"
#include "APG/font/SDLFontManager.hpp"

namespace APG {

class SDLGame : public Game {
public:
	explicit SDLGame(const std::string &windowTitle, uint32_t windowWidth, uint32_t windowHeight,
	        uint32_t glContextMajor = 3, uint32_t glContextMinor = 2, uint32_t windowX = SDL_WINDOWPOS_UNDEFINED,
	        uint32_t windowY = SDL_WINDOWPOS_UNDEFINED);
	virtual ~SDLGame();

	bool update(float deltaTime) override;
	void quit();

	SDL_Window *getWindow() const {
		return window.get();
	}

	SDL_GLContext getGLContext() const {
		return glContext;
	}

	virtual const InputManager *input() const override {
		return inputManager.get();
	}

	virtual const AudioManager *audio() const override {
		return audioManager.get();
	}

	virtual const FontManager *font() const override {
		return fontManager.get();
	}

	/**
	 * Defaults to SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS.
	 *
	 * Without these defaults at least, initialization will probably fail.
	 */
	static uint32_t SDL_INIT_FLAGS;

	/**
	 * Defaults to IMG_INIT_PNG.
	 */
	static uint32_t SDL_IMAGE_INIT_FLAGS;

	/**
	 * Defaults to MIX_INIT_OGG
	 */
	static uint32_t SDL_MIXER_INIT_FLAGS;

	/**
	 * Defaults to SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL.
	 */
	static uint32_t SDL_WINDOW_FLAGS;

protected:
	SXXDL::window_ptr window = SXXDL::make_window_ptr(nullptr);
	SDL_GLContext glContext = nullptr;

	SDL_Event eventCache;

	bool shouldQuit = false;

	virtual void handleEvent(SDL_Event &event);

	std::unique_ptr<SDLInputManager> inputManager = nullptr;
	std::unique_ptr<SDLAudioManager> audioManager = nullptr;
	std::unique_ptr<SDLFontManager> fontManager = nullptr;

private:
	void logSDLVersions() const;
	void debugSDLVersion(el::Logger * const logger, const char *libraryName, const SDL_version &compiledVersion,
	        const SDL_version &linkedVersion) const;
};

}

#endif /* SDLGAME_HPP_ */
