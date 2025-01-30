#include "tpch.h"
#include "AudioManager.h"

namespace Thingy {

	AudioManager::AudioManager(std::vector<uint8_t>& buffer) : musicBuffer(buffer), volume(0), currentTrackNum(0), currentTrackPos(0), audioOpen(0) {
		SDL_Log("Audio Manager Constructor");
		spec.freq = MIX_DEFAULT_FREQUENCY;
		spec.format = MIX_DEFAULT_FORMAT;
		spec.channels = MIX_DEFAULT_CHANNELS;
		if (!Mix_OpenAudio(0, &spec)) {
			SDL_Log("Couldn't open audio: %s\n", SDL_GetError());
		}
		else {
			Mix_QuerySpec(&spec.freq, &spec.format, &spec.channels);
			SDL_Log("Opened audio at %d Hz %d bit%s %s audio buffer\n", spec.freq,
				(spec.format & 0xFF),
				(SDL_AUDIO_ISFLOAT(spec.format) ? " (float)" : ""),
				(spec.channels > 2) ? "surround" : (spec.channels > 1) ? "stereo" : "mono");
		}
		audioOpen = 1;
		
		Mix_VolumeMusic(volume);
	}

	AudioManager::~AudioManager() {
	}
	
	void AudioManager::CleanUp() {

		Mix_FreeMusic(music);
		if (audioOpen) {
			Mix_CloseAudio();
		}
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	}

	void AudioManager::UpdateTrackPos() {
		currentTrackPos = Mix_GetMusicPosition(music);
		if (currentTrackPos == Mix_MusicDuration(music)) {
			NextTrack();
		}
	}

	void AudioManager::ChangeVolume() {
		Mix_VolumeMusic(volume);
	}

	void AudioManager::ChangeMusicPos() {
		Mix_SetMusicPosition(currentTrackPos);
	}

	void AudioManager::ChangeMusic() {
		LoadMusic();
		Mix_PlayMusic(music, 0);
		Mix_PauseMusic();
	}

	void AudioManager::PauseMusic() {
		Mix_PauseMusic();
	}

	void AudioManager::ResumeMusic() {
		Mix_ResumeMusic();
	}
		
	void AudioManager::NextTrack() {
		if (currentTrackNum < queue.size()) {
			currentTrackNum++;
			ChangeMusic();

		}
		else if (queue.size() == 0 || queue.size() == 1) {
			currentTrackPos = 0;
			Mix_PlayMusic(music, 0);
			PauseMusic();
		} else if (currentTrackNum == queue.size()) {
			currentTrackNum = 0;
			ChangeMusic();
		}
	
	}

	void AudioManager::PrevTrack() {
		if (currentTrackPos < 5) {
			if (currentTrackNum > 0) {
				currentTrackNum--;
				ChangeMusic();
			} else if (currentTrackNum == 0) {
				Mix_RewindMusic();
			}
		} else {
			Mix_RewindMusic();
		}
	}

	void AudioManager::LoadMusic() {
		SDL_Log("Music loading");
		
		if (musicBuffer.empty()) {
			SDL_Log("Buffer is empty, cannot load music.");
			return;
		}
		SDL_Log("Downloaded music file, size: %zu bytes", musicBuffer.size());
		if (music) {
			Mix_FreeMusic(music);
			music = nullptr;
		}
		music = LoadMusicFromMemory(musicBuffer);
		if (!music) {
			SDL_Log("Mix_LoadMUS_IO failed: %s\n", SDL_GetError());
		}
		SDL_Log("Music Loaded");
	}

	
}
