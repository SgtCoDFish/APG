#ifndef APG_NO_SDL

#include <cstdint>

#include "APG/SDL.hpp"

#include "APG/input/SDLInputManager.hpp"

namespace APG {

SDLInputManager::SDLInputManager() : logger{spdlog::get("APG")} {
	keyState = SDL_GetKeyboardState(nullptr);

	for (auto &f : canJustPress) {
		f = true;
	}
}

void SDLInputManager::update(float deltaTime) {
	for (auto &f : justPressed) {
		f = false;
	}

	mouseLeftJustPressed = false;
	mouseRightJustPressed = false;

	mods = SDL_GetModState();
}

void SDLInputManager::handleInputEvent(SDL_Event &event) {
	const auto &key = event.key.keysym.scancode;
	const auto &mouseMotion = event.motion;
	const auto &mouseButton = event.button;

	if (event.type == SDL_KEYDOWN && canJustPress[key]) {
		justPressed[key] = true;
		canJustPress[key] = false;
	} else if (event.type == SDL_KEYUP) {
		canJustPress[key] = true;
	} else if (event.type == SDL_MOUSEMOTION) {
		mouseX = mouseMotion.x;
		mouseY = mouseMotion.y;
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		if (mouseButton.button == SDL_BUTTON_LEFT) {
			mouseLeftPressed = true;

			if (mouseLeftCanJustPress) {
				mouseLeftJustPressed = true;
			}
		} else if (mouseButton.button == SDL_BUTTON_RIGHT) {
			mouseRightPressed = true;

			if (mouseRightCanJustPress) {
				mouseRightJustPressed = true;
			}
		}
	} else if (event.type == SDL_MOUSEBUTTONUP) {
		if (mouseButton.button == SDL_BUTTON_LEFT) {
			mouseLeftPressed = false;
			mouseLeftJustPressed = false;
			mouseLeftCanJustPress = true;
		} else if (mouseButton.button == SDL_BUTTON_RIGHT) {
			mouseRightPressed = false;
			mouseRightJustPressed = false;
			mouseRightCanJustPress = true;
		}
	}
}

bool SDLInputManager::isKeyPressed(const SDL_Scancode &key) const {
	return isSDLKeyCodePressed(key);
}

bool SDLInputManager::isKeyJustPressed(const SDL_Scancode &key) const {
	return justPressed[key];
}

bool SDLInputManager::isModPressed(const SDL_Keymod &mod) const {
	return (mods & mod) == mod;
}

bool SDLInputManager::isCtrlPressed() const {
	return (mods & KMOD_CTRL) != 0;
}

bool SDLInputManager::isShiftPressed() const {
	return (mods & KMOD_SHIFT) != 0;
}

bool SDLInputManager::isAltPressed() const {
	return (mods & KMOD_ALT) != 0;
}

bool SDLInputManager::isLeftMousePressed() const {
	return mouseLeftPressed;
}

bool SDLInputManager::isRightMousePressed() const {
	return mouseRightPressed;
}

bool SDLInputManager::isLeftMouseJustPressed() const {
	return mouseLeftJustPressed;
}

bool SDLInputManager::isRightMouseJustPressed() const {
	return mouseRightJustPressed;
}

bool SDLInputManager::isSDLKeyCodePressed(const SDL_Scancode &keysym) const {
	return keyState[keysym];
}

int32_t SDLInputManager::getMouseX() const {
	return mouseX;
}

int32_t SDLInputManager::getMouseY() const {
	return mouseY;
}

}

#endif
