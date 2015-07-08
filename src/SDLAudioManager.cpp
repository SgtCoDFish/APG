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

#include "APG/APGeasylogging.hpp"

#include "APG/SDLAudioManager.hpp"
#include "APG/internal/Assert.hpp"

APG::SDLAudioManager::SDLAudioManager(int frequency, uint16_t format, int channelCount) :
		APG::AudioManager() {
	const auto logger = el::Loggers::getLogger("default");

	if (Mix_OpenAudio(frequency, format, 2, 1024) == -1) {
		logger->fatal("Couldn't open audio for SDL_mixer: %v", Mix_GetError());
		return;
	}

	Mix_AllocateChannels(channelCount);

	logger->info("Successfully started SDL_mixer with %v channels.", channelCount);
}

APG::SDLAudioManager::~SDLAudioManager() {
	// Deallocates all channels.
	Mix_AllocateChannels(0);
	Mix_CloseAudio();
}

APG::AudioManager::music_handle APG::SDLAudioManager::loadMusicFile(const std::string &filename) {
	const auto logger = el::Loggers::getLogger("default");
	logger->info("Loading music file \"%v\".", filename);

	auto sdlMusic = SXXDL::mixer::make_music_ptr(Mix_LoadMUS(filename.c_str()));

	if (sdlMusic == nullptr) {
		logger->fatal("Couldn't load \"%v\", error: %v.", filename, Mix_GetError());
		return -1;
	} else {
		const auto handle = getNextMusicHandle();
		logger->info("Loaded \"%v\" at handle %v.", filename, handle);

		loadedMusic.emplace(handle, std::move(sdlMusic));

		logger->verbose(9, "Total loaded music files: %v", loadedMusic.size());

		return handle;
	}
}

APG::AudioManager::sound_handle APG::SDLAudioManager::loadSoundFile(const std::string &filename) {
	const auto logger = el::Loggers::getLogger("default");
	logger->info("Loading sound file \"%v\".", filename);

	auto sdlSound = SXXDL::mixer::make_sound_ptr(Mix_LoadWAV(filename.c_str()));

	if (sdlSound == nullptr) {
		logger->fatal("Couldn't load \"%v\", error: %v.", filename, Mix_GetError());
		return -1;
	} else {
		const auto handle = getNextSoundHandle();
		logger->info("Loaded \"%v\" at handle %v.", filename, handle);

		loadedSounds.emplace(handle, std::move(sdlSound));

		logger->verbose(9, "Total loaded sound files: %v.", loadedSounds.size());

		return handle;
	}
}

APG::AudioManager *APG::SDLAudioManager::setGlobalVolume(float volume) {
	REQUIRE(volume >= 0.0f && volume <= 1.0f, "Volume must be in the range 0.0f <= volume <= 1.0f.");
	return setMusicVolume(volume)->setSoundVolume(volume);
}

APG::AudioManager *APG::SDLAudioManager::setMusicVolume(float volume) {
	REQUIRE(volume >= 0.0f && volume <= 1.0f, "Volume must be in the range 0.0f <= volume <= 1.0f.");

	Mix_VolumeMusic((int) MIX_MAX_VOLUME * volume);

	return this;
}
APG::AudioManager *APG::SDLAudioManager::setSoundVolume(float volume) {
	REQUIRE(volume >= 0.0f && volume <= 1.0f, "Volume must be in the range 0.0f <= volume <= 1.0f.");

	Mix_Volume(-1, (int) MIX_MAX_VOLUME * volume);

	return this;
}

void APG::SDLAudioManager::freeMusic(music_handle &handle) {
	loadedMusic.erase(handle);
	handle = -1;
}

void APG::SDLAudioManager::freeSound(sound_handle &handle) {
	loadedSounds.erase(handle);
	handle = -1;
}

void APG::SDLAudioManager::playMusic(const music_handle &handle) {
	const auto &song = loadedMusic.find(handle);

	Mix_PlayMusic((*song).second.get(), 0);
}

void APG::SDLAudioManager::playSound(const sound_handle &handle) {
	const auto &song = loadedSounds.find(handle);

	if (Mix_PlayChannel(-1, (*song).second.get(), 0) == -1) {
		el::Loggers::getLogger("default")->warn("Couldn't play sound at handle %v because: %v", handle,
		Mix_GetError());
	}
}

void APG::SDLAudioManager::pauseMusic() {
	Mix_PauseMusic();
}

void APG::SDLAudioManager::resumeMusic() {
	Mix_ResumeMusic();
}

void APG::SDLAudioManager::stopMusic() {
	Mix_HaltMusic();
}

Mix_Music * APG::SDLAudioManager::getRawMusicPointer(const music_handle &handle) {
	return (*loadedMusic.find(handle)).second.get();
}

Mix_Chunk * APG::SDLAudioManager::getRawChunkPointer(const sound_handle &handle) {
	return (*loadedSounds.find(handle)).second.get();
}
