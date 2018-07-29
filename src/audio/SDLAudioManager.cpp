#ifndef APG_NO_SDL

#include "APG/internal/Assert.hpp"
#include "APG/audio/SDLAudioManager.hpp"

namespace APG {

APG::SDLAudioManager::SDLAudioManager(int frequency, uint16_t format, int channelCount) :
		APG::AudioManager(),
		logger{spdlog::get("APG")} {
	if (Mix_OpenAudio(frequency, format, 2, 1024) == -1) {
		logger->critical("Couldn't open audio for SDL_mixer: {}", Mix_GetError());
		throw std::runtime_error("Couldn't open SDL_mixer audio");
		return;
	}

	Mix_AllocateChannels(channelCount);

	logger->trace("Successfully started SDL_mixer with {} channels.", channelCount);
}

APG::SDLAudioManager::~SDLAudioManager() {
	// Deallocates all channels.
	Mix_AllocateChannels(0);
	Mix_CloseAudio();
}

APG::AudioManager::music_handle APG::SDLAudioManager::loadMusicFile(const std::string &filename) {
	logger->info("Loading music file \"{}\".", filename);

	auto sdlMusic = SXXDL::mixer::make_music_ptr(Mix_LoadMUS(filename.c_str()));

	if (sdlMusic == nullptr) {
		logger->critical("Couldn't load \"{}\", error: {}.", filename, Mix_GetError());
		return -1;
	} else {
		const auto handle = getNextMusicHandle();
		logger->info("Loaded \"{}\" at handle {}.", filename, handle);

		loadedMusic.emplace(handle, std::move(sdlMusic));

		logger->trace("Total loaded music files: {}", loadedMusic.size());

		return handle;
	}
}

APG::AudioManager::sound_handle APG::SDLAudioManager::loadSoundFile(const std::string &filename) {
	logger->info("Loading sound file \"{}\".", filename);

	auto sdlSound = SXXDL::mixer::make_sound_ptr(Mix_LoadWAV(filename.c_str()));

	if (sdlSound == nullptr) {
		logger->critical("Couldn't load \"{}\", error: {}.", filename, Mix_GetError());
		return -1;
	} else {
		const auto handle = getNextSoundHandle();
		logger->info("Loaded \"{}\" at handle {}.", filename, handle);

		loadedSounds.emplace(handle, std::move(sdlSound));

		logger->trace("Total loaded sound files: {}.", loadedSounds.size());

		return handle;
	}
}

AudioManager *SDLAudioManager::setGlobalVolume(float volume) {
	REQUIRE(volume >= 0.0f && volume <= 1.0f, "Volume must be in the range 0.0f <= volume <= 1.0f.");
	return setMusicVolume(volume)->setSoundVolume(volume);
}

AudioManager *SDLAudioManager::setMusicVolume(float volume) {
	REQUIRE(volume >= 0.0f && volume <= 1.0f, "Volume must be in the range 0.0f <= volume <= 1.0f.");

	Mix_VolumeMusic((int) MIX_MAX_VOLUME * volume);

	return this;
}
AudioManager *SDLAudioManager::setSoundVolume(float volume) {
	REQUIRE(volume >= 0.0f && volume <= 1.0f, "Volume must be in the range 0.0f <= volume <= 1.0f.");

	Mix_Volume(-1, (int) MIX_MAX_VOLUME * volume);

	return this;
}

void SDLAudioManager::freeMusic(music_handle &handle) {
	loadedMusic.erase(handle);
	freeMusicHandle(handle);
	handle = -1;
}

void SDLAudioManager::freeSound(sound_handle &handle) {
	loadedSounds.erase(handle);
	freeSoundHandle(handle);
	handle = -1;
}

void SDLAudioManager::playMusic(const music_handle &handle) {
	const auto &song = loadedMusic.find(handle);

	Mix_PlayMusic((*song).second.get(), 0);
}

void SDLAudioManager::playSound(const sound_handle &handle) {
	const auto &song = loadedSounds.find(handle);

	if (Mix_PlayChannel(-1, (*song).second.get(), 0) == -1) {
		logger->warn("Couldn't play sound at handle {} because: {}", handle, Mix_GetError());
	}
}

void SDLAudioManager::pauseMusic() {
	Mix_PauseMusic();
}

void SDLAudioManager::resumeMusic() {
	Mix_ResumeMusic();
}

void SDLAudioManager::stopMusic() {
	Mix_HaltMusic();
}

Mix_Music * SDLAudioManager::getRawMusicPointer(const music_handle &handle) {
	return (*loadedMusic.find(handle)).second.get();
}

Mix_Chunk * SDLAudioManager::getRawChunkPointer(const sound_handle &handle) {
	return (*loadedSounds.find(handle)).second.get();
}

}

#endif
