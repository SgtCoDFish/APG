#ifndef INCLUDE_APG_AUDIO_SDLAUDIOMANAGER_HPP_
#define INCLUDE_APG_AUDIO_SDLAUDIOMANAGER_HPP_

#ifndef APG_NO_SDL

#include <unordered_map>

#include "spdlog/spdlog.h"

#include "APG/SDL.hpp"

#include "APG/SXXDL.hpp"
#include "APG/audio/Audio.hpp"

namespace APG {

class SDLAudioManager : public AudioManager {
public:
	explicit SDLAudioManager(int frequency = MIX_DEFAULT_FREQUENCY, uint16_t format = MIX_DEFAULT_FORMAT,
	        int channelCount = 8);
	virtual ~SDLAudioManager();

	virtual music_handle loadMusicFile(const std::string &filename) override;
	virtual sound_handle loadSoundFile(const std::string &filename) override;

	virtual APG::AudioManager * setGlobalVolume(float volume) override;
	virtual APG::AudioManager * setMusicVolume(float volume) override;
	virtual APG::AudioManager * setSoundVolume(float volume) override;

	virtual void freeMusic(music_handle &handle) override;
	virtual void freeSound(sound_handle &handle) override;

	virtual void playMusic(const music_handle &handle) override;
	virtual void playSound(const sound_handle &handle) override;

	virtual void pauseMusic() override;
	virtual void resumeMusic() override;
	virtual void stopMusic() override;

	/**
	 * @return a raw music pointer to the underlying SDL data; use with caution as this could conflict with the audio manager's use of the object.
	 */
	Mix_Music * getRawMusicPointer(const music_handle &handle);

	/**
	 * @return a raw chunk pointer to the underlying SDL data; use with caution as this could conflict with the audio manager's use of the object.
	 */
	Mix_Chunk * getRawChunkPointer(const sound_handle &handle);

private:
	std::unordered_map<music_handle, SXXDL::mixer::music_ptr> loadedMusic;
	std::unordered_map<sound_handle, SXXDL::mixer::sound_ptr> loadedSounds;

	std::shared_ptr<spdlog::logger> logger;
};

}

#endif

#endif
