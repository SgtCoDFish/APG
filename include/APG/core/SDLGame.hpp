#ifndef INCLUDE_APG_CORE_SDLGAME_HPP_
#define INCLUDE_APG_CORE_SDLGAME_HPP_

#ifndef APG_NO_SDL

#include <cstdint>

#include "APG/SDL.hpp"
#include "APG/GL.hpp"

#include "APG/SXXDL.hpp"
#include "APG/core/Game.hpp"
#include "APG/core/APGContext.hpp"
#include "APG/input/SDLInputManager.hpp"
#include "APG/audio/SDLAudioManager.hpp"
#include "APG/font/PackedFontManager.hpp"

namespace APG {

class SDLGame : public Game {
public:
	explicit SDLGame(APGContext &apgContext);
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

protected:
	APGContext &context;
	SXXDL::window_ptr window = SXXDL::make_window_ptr(nullptr);
	SDL_GLContext glContext = nullptr;

	SDL_Event eventCache;

	bool shouldQuit = false;

	virtual void handleEvent(SDL_Event &event);

	std::unique_ptr<SDLInputManager> inputManager = nullptr;
	std::unique_ptr<SDLAudioManager> audioManager = nullptr;
	std::unique_ptr<PackedFontManager> fontManager = nullptr;

	void resetGLErrors();

	std::shared_ptr<spdlog::logger> logger;

private:
	static void logSDLVersions(std::shared_ptr<spdlog::logger> &logger);
	static void debugSDLVersion(std::shared_ptr<spdlog::logger> &logger, const char *libraryName, const SDL_version &compiledVersion,
	        const SDL_version &linkedVersion);
};

}

#endif

#endif
