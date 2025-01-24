#include "tpch.h"
#include "AudioManager.h"
#include <curl\curl.h>
#include <curl\easy.h>

namespace Thingy {
	size_t AudioManager::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
		size_t totalSize = size * nmemb;
		std::vector<char>* buffer = static_cast<std::vector<char>*>(userp);
		buffer->insert(buffer->end(), static_cast<char*>(contents), static_cast<char*>(contents) + totalSize);
		return totalSize;
	}

	bool AudioManager::DownloadFile(const std::string& url, std::vector<char>& buffer) {
		CURL* curl = curl_easy_init();
		if (!curl) return false;

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &AudioManager::WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		return (res == CURLE_OK);
	}

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
		//Mix_HookMusicFinished(OnMusicFinished());
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
		

	}

	void AudioManager::PauseMusic() {
		Mix_PauseMusic();
	}

	void AudioManager::StartMusic() {
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
		if (currentTrackNum > 0) {
			currentTrackNum--;
			ChangeMusic();
		} else if (currentTrackNum == 0) {
			Mix_RewindMusic();
		}
	}
	void AudioManager::LoadMusic() {
		std::vector<char> musicBuffer;
		std::string musicURL = "https:\/\/prod-1.storage.jamendo.com\/?trackid=1848357&format=mp31&from=app-devsite";

		if (DownloadFile(musicURL, musicBuffer)) {
			music = LoadMusicFromMemory(musicBuffer);
			if (music) {
				if (Mix_PlayMusic(music, -1) == -1) {
					SDL_Log("Failed to play music: %s\n", SDL_GetError());
				}
			}
		}
		else {
			SDL_Log("Failed to download music from URL.\n");
		}
	}

	Mix_Music* AudioManager::LoadMusicFromMemory(const std::vector<char>& buffer) {
		SDL_IOStream* ioStream = SDL_IOFromConstMem(buffer.data(), buffer.size());
		if (!ioStream) {
			SDL_Log("Failed to create RWops: %s\n", SDL_GetError());
			return nullptr;
		}

		Mix_Music* music = Mix_LoadMUS_IO(ioStream, 1);
		if (!music) {
			SDL_Log("Failed to load music: %s\n", SDL_GetError());
		}
		return music;

	}
}
