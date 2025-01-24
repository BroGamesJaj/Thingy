#include "tpch.h"
#include "AudioManager.h"

namespace Thingy {

	

	AudioManager::AudioManager() {
		SDL_Log("Audio Manager Constructor");
		Init();
	}

	AudioManager::~AudioManager() {
		Mix_FreeMusic(music);
		if (audioOpen) {
			Mix_CloseAudio();
		}	
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		
	}

	void AudioManager::Init() {
		volume = 0;
		currentTrackNum = 0;
		currentTrackPos = 0;
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
		Mix_HookMusicFinished(OnMusicFinished());
	}

	Mix_MusicFinishedCallback AudioManager::OnMusicFinished() {
		SDL_Log("Music ended");
		//NextTrack();
		return 0;
	}
	
	void AudioManager::UpdateTrackPos() {
		currentTrackPos = Mix_GetMusicPosition(music);
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
			
		} else if (currentTrackNum == queue.size()) {
			currentTrackNum = 0;
			ChangeMusic();
			PauseMusic();
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
		musicBuffer.clear();
		std::string musicURL = "https:\/\/prod-1.storage.jamendo.com\/?trackid=1848357&format=mp31&from=app-devsite";

		
		if (DownloadFile(musicURL, musicBuffer)) {
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
				SDL_Log("Mix_LoadMUS_RW failed: %s\n", SDL_GetError());
			}
		}
		else {
			SDL_Log("Failed to download music from URL.\n");
		}
		
	}

	
}
