#ifndef INCLUDE_APG_INPUT_SDLINPUTMANAGER_HPP_
#define INCLUDE_APG_INPUT_SDLINPUTMANAGER_HPP_

#ifndef APG_NO_SDL

#include <cstdint>

#include <array>

#include <spdlog/spdlog.h>

#include "APG/SDL.hpp"

#include "APG/input/InputManager.hpp"

namespace APG {

class SDLInputManager : public InputManager {
public:
	explicit SDLInputManager();

	~SDLInputManager() override = default;

	void update(float deltaTime) override;

	bool isKeyPressed(const SDL_Scancode &key) const override;

	bool isKeyJustPressed(const SDL_Scancode &key) const override;

	bool isModPressed(const SDL_Keymod &mod) const override;

	bool isCtrlPressed() const override;

	bool isShiftPressed() const override;

	bool isAltPressed() const override;

	int32_t getMouseX() const override;

	int32_t getMouseY() const override;

	bool isLeftMousePressed() const override;

	bool isRightMousePressed() const override;

	bool isLeftMouseJustPressed() const override;

	bool isRightMouseJustPressed() const override;

	void handleInputEvent(SDL_Event &event);

	bool isSDLKeyCodePressed(const SDL_Scancode &keysym) const;

private:
	std::array<bool, SDL_Scancode::SDL_NUM_SCANCODES> justPressed;
	std::array<bool, SDL_Scancode::SDL_NUM_SCANCODES> canJustPress;

	const uint8_t *keyState = nullptr;
	uint64_t mods = SDL_Keymod::KMOD_NONE;

	bool mouseLeftCanJustPress{true};
	bool mouseRightCanJustPress{true};

	bool mouseLeftPressed{false};
	bool mouseRightPressed{false};
	bool mouseLeftJustPressed{false};
	bool mouseRightJustPressed{false};

	int32_t mouseX{0};
	int32_t mouseY{0};

	std::shared_ptr<spdlog::logger> logger;
};

}

#endif

#endif
