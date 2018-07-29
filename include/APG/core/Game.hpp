#ifndef INCLUDE_APG_CORE_GAME_HPP_
#define INCLUDE_APG_CORE_GAME_HPP_

#include <cstdint>

#include <string>

namespace APG {
class InputManager;
class AudioManager;
class FontManager;

/**
 * Note that only one Game is expected to be made, and the public static screen size variables will be wrong if you create more than one.
 */
class Game {
public:
	static uint32_t screenWidth;
	static uint32_t screenHeight;

	static void setLoggerToAPGStyle(const std::string &loggerName);
	static void setupLoggingDefault();

	explicit Game(uint32_t screenWidth, uint32_t screenHeight);

	virtual ~Game() = default;

	/**
	 * Should carry out initialisation of the game.
	 * @return true if successful, false if an error state has been set.
	 */
	virtual bool init() = 0;

	/**
	 * Called every frame. Should call render.
	 * @param deltaTime the amount of time elapsed since the last frame.
	 * @return true if it's time to quit, false otherwise.
	 */
	virtual bool update(float deltaTime) = 0;

	/**
	 * Should be called in update(float) every frame and render the whole visible screen.
	 * @param deltaTime the amount of time elapsed since the last frame.
	 */
	virtual void render(float deltaTime) = 0;

	virtual const InputManager *input() const = 0;
	virtual const AudioManager *audio() const = 0;
	virtual const FontManager *font() const = 0;
};

}

#endif /* APGGAME_HPP_ */
