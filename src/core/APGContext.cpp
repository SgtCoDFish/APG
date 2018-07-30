#include "APG/core/APGContext.hpp"

namespace APG {

APGContextBuilder::APGContextBuilder(std::string windowTitle,
        uint32_t windowWidth, uint32_t windowHeight) :
    windowTitle{std::move(windowTitle)},
    windowWidth{windowWidth},
    windowHeight{windowHeight} {
}


APGContextBuilder &APGContextBuilder::setLogLevel(const spdlog::level::level_enum &level) {
    this->logLevel = level;
    return *this;
}

APGContextBuilder &APGContextBuilder::setWindowPos(uint32_t windowX, uint32_t windowY) {
    this->windowX = windowX;
    this->windowY = windowY;
    return *this;
}

APGContextBuilder &APGContextBuilder::setGLContextVersion(uint32_t glContextMajor, uint32_t glContextMinor) {
    this->glContextMajor = glContextMajor;
    this->glContextMinor = glContextMinor;
    return *this;
}

APGContextBuilder &APGContextBuilder::addAdditionalSDLInitFlags(uint32_t additionalFlags) {
    this->sdl2InitFlags |= additionalFlags;
    return *this;
}

APGContextBuilder &APGContextBuilder::addAdditionalSDLImageInitFlags(uint32_t additionalFlags) {
    this->sdl2ImageInitFlags |= additionalFlags;
    return *this;
}

APGContextBuilder &APGContextBuilder::addAdditionalSDLMixerInitFlags(uint32_t additionalFlags) {
    this->sdl2MixerInitFlags |= additionalFlags;
    return *this;
}

APGContextBuilder &APGContextBuilder::addAdditionalSDLWindowInitFlags(uint32_t additionalFlags) {
    this->sdl2WindowInitFlags |= additionalFlags;
    return *this;
}

APGContext APGContextBuilder::build() {
    auto context = APGContext(windowTitle, windowWidth, windowHeight, windowX, windowY, glContextMajor, glContextMinor,
                logLevel, sdl2InitFlags, sdl2ImageInitFlags, sdl2MixerInitFlags, sdl2WindowInitFlags);
    return context;
}

APGContext::APGContext(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight, uint32_t windowX, uint32_t windowY,
						uint32_t glContextMajor, uint32_t glContextMinor,
						spdlog::level::level_enum logLevel,
						uint32_t sdl2InitFlags, uint32_t sdl2ImageInitFlags,
						uint32_t sdl2MixerInitFlags, uint32_t sdl2WindowInitFlags) :
                            windowTitle{std::move(windowTitle)},
                            windowWidth{windowWidth},
                            windowHeight{windowHeight},
                            windowX{windowX},
                            windowY{windowY},
                            glContextMajor{glContextMajor},
                            glContextMinor{glContextMinor},
                            logLevel{logLevel},
                            logger{spdlog::get("APG") == nullptr ? spdlog::stdout_logger_mt("APG") : spdlog::get("APG")},
                            sdl2InitFlags{sdl2InitFlags},
                            sdl2ImageInitFlags{sdl2ImageInitFlags},
                            sdl2MixerInitFlags{sdl2MixerInitFlags},
                            sdl2WindowInitFlags{sdl2WindowInitFlags} {
    logger->set_level(logLevel);
    initSDL();
}

APGContext::~APGContext() {
	SDLNet_Quit();
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void APGContext::initSDL() {
	if (SDL_Init(sdl2InitFlags) < 0) {
		logger->critical("Couldn't initialise SDL2: {}", SDL_GetError());
		throw std::runtime_error("Failed to initialise SDL2");
	} else {
		logger->trace("Successfully initialised SDL2.");
	}

	if ((IMG_Init(sdl2ImageInitFlags) & sdl2ImageInitFlags) != sdl2ImageInitFlags) {
		logger->critical("Couldn't initialise SDL2_image: {}", IMG_GetError());
		throw std::runtime_error("Failed to initialise SDL2_image");
	} else {
		logger->trace("Successfully initialised SDL2_image.");
	}

	if (TTF_Init() == -1) {
		logger->critical("Couldn't initialise SDL2_ttf: {}", TTF_GetError());
		throw std::runtime_error("Failed to initialise SDL2_ttf");
	} else {
		logger->trace("Successfully initialised SDL2_ttf.");
	}

	if ((Mix_Init(sdl2MixerInitFlags) & sdl2MixerInitFlags) != sdl2MixerInitFlags) {
		logger->critical("Couldn't initialise SDL2_mixer: {}", Mix_GetError());
		throw std::runtime_error("Failed to initialise SDL2_mixer");
	} else {
		logger->trace("Successfully initialised SDL2_mixer.");
	}

	if (SDLNet_Init() == -1) {
		logger->critical("Couldn't initialise SDL2_net: {}", SDLNet_GetError());
		throw std::runtime_error("Failed to initialise SDL2_net");
	} else {
		logger->trace("Successfully initialised SDL2_net");
	}

}

}