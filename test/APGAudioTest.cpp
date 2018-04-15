#include <chrono>

#include "APG/core/SDLGame.hpp"
#include "test/APGAudioTest.hpp"

#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

bool APG::APGAudioTest::init() {
	soundHandle = audioManager->loadSoundFile("assets/test_sound.wav");

	spriteBatch = std::make_unique<SpriteBatch>();

	font = fontManager->loadFontFile("assets/test_font.ttf", 12);
	fontSprite =
	        fontManager->renderText(font,
	                "To test:\nP: load/unload music\nSpace: play from beginning (if loaded)\nA: pause (if playing)\nD: resume (if paused)\nEnter: play test sound\nNumber keys: change volume",
	                false, FontRenderMethod::NICE);

	return true;
}

void APG::APGAudioTest::render(float deltaTime) {
	(testHandle == -1 ? clearToRed() : clearToGreen());
	glClear(GL_COLOR_BUFFER_BIT);

	// Don't do this in production, but it's usefully hacky for testing.
	for (int i = SDL_SCANCODE_1; i < SDL_SCANCODE_0; ++i) {
		if (inputManager->isKeyJustPressed((SDL_Scancode) i)) {
			const float volume = ((float) ((i - SDL_SCANCODE_1) + 1)) / 10.0f;
			el::Loggers::getLogger("APG")->info("Setting volume to %v", volume);

			audioManager->setGlobalVolume(volume);

			break;
		}
	}

	if (inputManager->isKeyJustPressed(SDL_SCANCODE_0)) {
		el::Loggers::getLogger("APG")->info("Setting volume to max");

		audioManager->setGlobalVolume(1.0f);
	}

	if (inputManager->isKeyJustPressed(SDL_SCANCODE_P)) {
		if (testHandle == -1) {
			testHandle = audioManager->loadMusicFile("assets/test_music.ogg");
		} else {
			audioManager->freeMusic(testHandle);
		}
	}

	if (inputManager->isKeyJustPressed(SDL_SCANCODE_A)) {
		audioManager->pauseMusic();
	} else if (inputManager->isKeyJustPressed(SDL_SCANCODE_D)) {
		audioManager->resumeMusic();
	}

	if (inputManager->isKeyJustPressed(SDL_SCANCODE_SPACE) && testHandle != -1) {
		audioManager->playMusic(testHandle);
	}

	if (inputManager->isKeyJustPressed(SDL_SCANCODE_RETURN)) {
		audioManager->playSound(soundHandle);
	}

	if (inputManager->isCtrlPressed() && inputManager->isKeyJustPressed(SDL_SCANCODE_Q)) {
		quit();
	}

	spriteBatch->begin();
	spriteBatch->draw(fontSprite, 5.0f, 5.0f);
	spriteBatch->end();

	SDL_GL_SwapWindow(window.get());
}

void APG::APGAudioTest::clearToRed() {
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

void APG::APGAudioTest::clearToGreen() {
	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
}

int main(int argc, char **argv) {
	START_EASYLOGGINGPP(argc, argv);

	const auto logger = el::Loggers::getLogger("APG");

	{
		const auto game = std::make_unique<APG::APGAudioTest>();
		if (!game->init()) {
			logger->fatal("Couldn't init audio test.");
		}

		auto startTime = std::chrono::high_resolution_clock::now();

		logger->info(
		        "To test:\n\t> P to load/unload music\n\t> Space to play from beginning if loaded\n\t> A to pause if playing\n\t> D to resume if paused\n\t> Enter to play test sound.\n\t> Number keys to change volume");

		while (true) {
			const auto timeNow = std::chrono::high_resolution_clock::now();
			const float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - startTime).count()
			        / 1000.0f;

			if (game->update(deltaTime)) {
				break;
			}
		}
	}

	return EXIT_SUCCESS;
}
