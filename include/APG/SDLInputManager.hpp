/*
 * Copyright (c) 2015 Ashley Davis (SgtCoDFish)
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

#ifndef INCLUDE_APG_SDLINPUTMANAGER_HPP_
#define INCLUDE_APG_SDLINPUTMANAGER_HPP_

#include <cstdint>

#include <array>

#include <SDL2/SDL.h>

#include "APG/InputManager.hpp"

namespace APG {

class SDLInputManager : public InputManager {
private:
	std::array<bool, SDL_Scancode::SDL_NUM_SCANCODES> justPressed;
	std::array<bool, SDL_Scancode::SDL_NUM_SCANCODES> canJustPress;

	const uint8_t *keyState = nullptr;
	SDL_Keymod mods = SDL_Keymod::KMOD_NONE;

public:
	explicit SDLInputManager();
	virtual ~SDLInputManager() = default;

	virtual void update(float deltaTime) override;

	virtual bool isKeyPressed(const SDL_Scancode &key) const override;
	virtual bool isKeyJustPressed(const SDL_Scancode &key) const override;

	virtual bool isModPressed(const SDL_Keymod &mod) const override;

	virtual bool isCtrlPressed() const override;
	virtual bool isShiftPressed() const override;
	virtual bool isAltPressed() const override;

	void handleInputEvent(SDL_Event &event);

	bool isSDLKeyCodePressed(const SDL_Scancode &keysym) const;
};

}

#endif /* INCLUDE_APG_SDLINPUTMANAGER_HPP_ */
