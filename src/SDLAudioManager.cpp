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

#include "easylogging++.h"

#include "APG/SDLAudioManager.hpp"

APG::SDLAudioManager::SDLAudioManager(int frequency, uint16_t format) {
	const auto logger = el::Loggers::getLogger("default");
	if (Mix_OpenAudio(frequency, format, 2, 1024) == -1) {
		logger->fatal("Couldn't open audio for SDL_mixer: %v", Mix_GetError());
		return;
	}

	logger->info("Successfully started SDL_mixer.");
}

APG::SDLAudioManager::~SDLAudioManager() {
	Mix_CloseAudio();
}

APG::AudioManager::music_handle APG::SDLAudioManager::loadMusicFile(const std::string &filename) {
	const auto logger = el::Loggers::getLogger("default");
	logger->info("Loading music file \"%v\".", filename);

	const auto handle = getNextMusicHandle();

	// LOAD

	return handle;
}

APG::AudioManager::sound_handle APG::SDLAudioManager::loadSoundFile(const std::string &filename) {
	const auto logger = el::Loggers::getLogger("default");
	logger->info("Loading sound file \"%v\".", filename);

	const auto handle = getNextSoundHandle();

	// LOAD

	return handle;
}

void APG::SDLAudioManager::freeMusic(const music_handle &handle) {
	// FREE
}

void APG::SDLAudioManager::freeSound(const sound_handle &handle) {
	// FREE
}
