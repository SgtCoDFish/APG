/*
 * Copyright (c) 2015 See AUTHORS file.
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

#include <cstdint>

#include <string>
#include <deque>

#include "APG/core/APGCommon.hpp"
#include "APG/audio/Audio.hpp"

APG::AudioManager::AudioManager(int initialMusicHandleCount, int initialSoundHandleCount) {
	fillDefaultQueues(initialMusicHandleCount, initialSoundHandleCount);
}

APG::AudioManager::music_handle APG::AudioManager::getNextMusicHandle() {
	const auto retval = availableMusicHandles.front();
	availableMusicHandles.pop_front();
	return retval;
}

APG::AudioManager::sound_handle APG::AudioManager::getNextSoundHandle() {
	const auto retval = availableSoundHandles.front();
	availableSoundHandles.pop_front();
	return retval;
}

void APG::AudioManager::freeMusicHandle(music_handle handle) {
	availableMusicHandles.push_front(handle);
}

void APG::AudioManager::freeSoundHandle(sound_handle handle) {
	availableSoundHandles.push_front(handle);
}

void APG::AudioManager::fillDefaultQueues(int initialMusicHandleCount, int initialSoundHandleCount) {
	for (music_handle i = 0; i < initialMusicHandleCount; ++i) {
		availableMusicHandles.push_back(i);
	}

	for (sound_handle i = 0; i < initialSoundHandleCount; ++i) {
		availableSoundHandles.push_back(i);
	}
}
