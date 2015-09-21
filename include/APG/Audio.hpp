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

#ifndef INCLUDE_APG_AUDIO_HPP_
#define INCLUDE_APG_AUDIO_HPP_

#include <cstdint>

#include <string>
#include <deque>
#include <type_traits>

#include "APG/APGCommon.hpp"

namespace APG {

/**
 * Handles everything audio related in the engine. Loaded music and sounds will persist until the manager is destroyed;
 * if you're running into memory issues, you can delete sooner however.
 *
 * Note that loading music and sounds depends on the implementation of AudioManager used and the file types supported may differ between implementation.
 *
 * The default handle returning implementation supports up to 256 sound handles and 128 music handles but you can change this if you want.
 * This isn't a hard limit, but there may be some slowdown after this point.
 */
class AudioManager {
public:
    using music_handle = int32_t;
    using sound_handle = int32_t;

    static_assert(std::is_copy_constructible<music_handle>(), "Music handle type must be copy constructible.");
    static_assert(std::is_copy_constructible<sound_handle>(), "Sound handle type must be copy constructible.");

private:
    std::deque<music_handle> availableMusicHandles;
    std::deque<sound_handle> availableSoundHandles;
    void fillDefaultQueues(int initialMusicHandleCount, int initalSoundHandleCount);

protected:
    music_handle getNextMusicHandle();
    sound_handle getNextSoundHandle();

    void freeMusicHandle(music_handle handle);
    void freeSoundHandle(sound_handle handle);

public:
    explicit AudioManager(int initialMusicHandleCount = internal::DEFAULT_MUSIC_HANDLE_COUNT,
            int initialSoundHandleCount = internal::DEFAULT_SOUND_HANDLE_COUNT);
    virtual ~AudioManager() = default;

    virtual music_handle loadMusicFile(const std::string &filename) = 0;
    virtual sound_handle loadSoundFile(const std::string &filename) = 0;

    virtual APG::AudioManager * setGlobalVolume(float volume) = 0;
    virtual APG::AudioManager * setMusicVolume(float volume) = 0;
    virtual APG::AudioManager * setSoundVolume(float volume) = 0;

    virtual void freeMusic(music_handle &handle) = 0;
    virtual void freeSound(sound_handle &handle) = 0;

    /**
     * Play the given music file from the start.
     * @param handle a valid handle returned from loadMusicFile.
     */
    virtual void playMusic(const music_handle &handle) = 0;
    virtual void playSound(const sound_handle &handle) = 0;

    virtual void pauseMusic() = 0;
    virtual void resumeMusic() = 0;
    virtual void stopMusic() = 0;
};

}

#endif /* INCLUDE_APG_AUDIO_HPP_ */
