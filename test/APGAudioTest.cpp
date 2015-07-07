/*
 * Copyright (c) 2014, 2015 Ashley Davis (SgtCoDFish)
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

#include <chrono>

#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

#include "APG/SDLGame.hpp"

#include "test/APGAudioTest.hpp"

bool APG::APGAudioTest::init() {
	return true;
}

void APG::APGAudioTest::render(float deltaTime) {
	(testHandle == -1 ? clearToRed() : clearToGreen());
	glClear(GL_COLOR_BUFFER_BIT);

	if (inputManager->isKeyJustPressed(SDL_SCANCODE_P)) {
		if (testHandle == -1) {
			testHandle = audioManager->loadMusicFile("assets/test_music.ogg");
		} else {
			audioManager->freeMusic(testHandle);
		}
	}

	if (inputManager->isKeyJustPressed(SDL_SCANCODE_SPACE) && testHandle != -1) {
		audioManager->playMusic(testHandle);
	}

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

	const auto logger = el::Loggers::getLogger("default");

	{
		const auto game = std::make_unique<APG::APGAudioTest>();
		if (!game->init()) {
			logger->fatal("Couldn't init audio test.");
		}

		auto startTime = std::chrono::high_resolution_clock::now();

		logger->info("To test: press P to load/unload music, space to play/pause/resume.");

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
