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

#ifndef INCLUDE_APG_SDLAUDIOMANAGER_HPP_
#define INCLUDE_APG_SDLAUDIOMANAGER_HPP_

#include <unordered_map>

#include <SDL2/SDL_mixer.h>

#include "APG/SXXDL.hpp"
#include "APG/Audio.hpp"

namespace APG {

class SDLAudioManager: public AudioManager {
private:
	std::unordered_map<music_handle, SXXDL::mixer::music_ptr> loadedMusic;
	std::unordered_map<sound_handle, SXXDL::mixer::sound_ptr> loadedSounds;

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
};

}

#endif /* INCLUDE_APG_SDLAUDIOMANAGER_HPP_ */
