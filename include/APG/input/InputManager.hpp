#ifndef INCLUDE_APG_INPUT_INPUTMANAGER_HPP_
#define INCLUDE_APG_INPUT_INPUTMANAGER_HPP_

#ifndef APG_NO_SDL

#include <cstdint>

#include "APG/SDL.hpp"

namespace APG {

class InputManager {
public:
	virtual ~InputManager() = default;

	virtual void update(float deltaTime) = 0;

	virtual bool isKeyPressed(const SDL_Scancode &key) const = 0;

	virtual bool isKeyJustPressed(const SDL_Scancode &key) const = 0;

	virtual bool isModPressed(const SDL_Keymod &mod) const = 0;

	virtual bool isCtrlPressed() const = 0;

	virtual bool isShiftPressed() const = 0;

	virtual bool isAltPressed() const = 0;

	virtual bool isLeftMousePressed() const = 0;

	virtual bool isRightMousePressed() const = 0;

	virtual bool isLeftMouseJustPressed() const = 0;

	virtual bool isRightMouseJustPressed() const = 0;

	virtual int32_t getMouseX() const = 0;

	virtual int32_t getMouseY() const = 0;

protected:
	static constexpr float JUST_PRESSED_THRESHOLD = 0.0001f;
};

}

#endif

#endif
