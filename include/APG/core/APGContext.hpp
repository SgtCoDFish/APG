#ifndef INCLUDE_APG_CORE_APGCONTEXT_HPP_
#define INCLUDE_APG_CORE_APGCONTEXT_HPP_

#include <cstdint>

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>

#include "spdlog/spdlog.h"

namespace APG {
class APGContextBuilder;

class APGContext final {
private:
    explicit APGContext(std::string windowTitle,
						uint32_t windowWidth, uint32_t windowHeight,
						uint32_t windowX, uint32_t windowY,
						uint32_t glContextMajor, uint32_t glContextMinor,
						spdlog::level::level_enum logLevel,
						uint32_t sdl2InitFlags,
						uint32_t sdl2ImageInitFlags,
						uint32_t sdl2MixerInitFlags,
						uint32_t sdl2WindowInitFlags);
	friend class APGContextBuilder;

	void initSDL();

public:
    ~APGContext();

	const std::string windowTitle;

	const uint32_t windowWidth;
	const uint32_t windowHeight;

    const uint32_t windowX;
    const uint32_t windowY;

    const uint32_t glContextMajor;
    const uint32_t glContextMinor;

	const spdlog::level::level_enum logLevel;
	const std::shared_ptr<spdlog::logger> logger;

	/**
	 * Always has SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS.
	 */
	const uint32_t sdl2InitFlags;

	/**
	 * Always has IMG_INIT_PNG.
	 */
	const uint32_t sdl2ImageInitFlags;

	/**
	 * Always has MIX_INIT_OGG
	 */
	const uint32_t sdl2MixerInitFlags;

	/**
	 * Always has SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL.
	 */
	const uint32_t sdl2WindowInitFlags;
};

class APGContextBuilder final {
public:
    explicit APGContextBuilder(std::string windowTitle,
  							   uint32_t windowWidth, uint32_t windowHeight);
    ~APGContextBuilder() = default;

    APGContextBuilder &setLogLevel(const spdlog::level::level_enum &level);
    APGContextBuilder &setWindowPos(uint32_t windowX, uint32_t windowY);
	APGContextBuilder &setGLContextVersion(uint32_t glContextMajor, uint32_t glContextMinor);

	APGContextBuilder &addAdditionalSDLInitFlags(uint32_t additionalFlags);
	APGContextBuilder &addAdditionalSDLImageInitFlags(uint32_t additionalFlags);
	APGContextBuilder &addAdditionalSDLMixerInitFlags(uint32_t additionalFlags);
	APGContextBuilder &addAdditionalSDLWindowInitFlags(uint32_t additionalFlags);

    APGContext build();

private:
	std::string windowTitle;

	uint32_t windowWidth;
	uint32_t windowHeight;

    uint32_t windowX {SDL_WINDOWPOS_UNDEFINED};
    uint32_t windowY {SDL_WINDOWPOS_UNDEFINED};

    uint32_t glContextMajor {3};
    uint32_t glContextMinor {2};

	spdlog::level::level_enum logLevel{spdlog::level::info};

	uint32_t sdl2InitFlags {SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS};
	uint32_t sdl2ImageInitFlags {IMG_INIT_PNG};
#if defined (__APPLE__)
	// TODO: Work out why this is required.
	uint32_t sdl2MixerInitFlags {0};
#else
	uint32_t sdl2MixerInitFlags {MIX_INIT_OGG};
#endif
	uint32_t sdl2WindowInitFlags {SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL};
};

}

#endif