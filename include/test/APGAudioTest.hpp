#ifndef INCLUDE_TEST_APGAUDIOTEST_HPP_
#define INCLUDE_TEST_APGAUDIOTEST_HPP_

#include <cstdint>

#include "APG/core/SDLGame.hpp"
#include "APG/graphics/SpriteBatch.hpp"

namespace APG {

class APGAudioTest final : public SDLGame {
public:
	explicit APGAudioTest(APGContext &context) :
			        SDLGame(context) {
	}

	virtual ~APGAudioTest() = default;

	bool init() override;
	void render(float deltaTime) override;

private:
	AudioManager::music_handle testHandle = -1;
	AudioManager::sound_handle soundHandle = -1;

	inline void clearToRed();
	inline void clearToGreen();

	std::unique_ptr<SpriteBatch> spriteBatch{nullptr};

	FontManager::font_handle font = -1;
	SpriteBase * fontSprite = nullptr;
};

}

#endif