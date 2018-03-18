#ifndef INCLUDE_APG_SDL_HPP_
#define INCLUDE_APG_SDL_HPP_

#ifndef APG_NO_SDL

#if defined(__EMSCRIPTEN__)
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>
#endif

#endif

#endif
