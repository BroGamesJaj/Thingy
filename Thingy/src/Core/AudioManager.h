#pragma once
#include "tpch.h"
#include "Track.h"
#include <fstream>
#include <stdio.h>
#include <SDL3\SDL.h>
#include <SDL3_mixer\SDL_mixer.h>

#include <curl\curl.h>
#include <curl\easy.h>

namespace Thingy {

	inline size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
		size_t totalSize = size * nmemb;
		std::vector<char>* buffer = static_cast<std::vector<char>*>(userp);
		buffer->insert(buffer->end(), static_cast<char*>(contents), static_cast<char*>(contents) + totalSize);
		return totalSize;
	}

	inline bool DownloadFile(const std::string& url, std::vector<char>& buffer) {
		CURL* curl = curl_easy_init();
		if (!curl) {
			SDL_Log("Failed to initialize CURL.");
			return false;
		}

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			SDL_Log("CURL failed: %s\n", curl_easy_strerror(res));
			buffer.clear();
		}
		curl_easy_cleanup(curl);

		return (res == CURLE_OK);
	}

	inline Mix_Music* LoadMusicFromMemory(const std::vector<char>& buffer) {
		SDL_IOStream* ioStream = SDL_IOFromConstMem(buffer.data(), buffer.size());
		if (!ioStream) {
			SDL_Log("Failed to create IOStream: %s\n", SDL_GetError());
			return nullptr;
		}

		Mix_Music* music = Mix_LoadMUS_IO(ioStream, 1);
		if (!music) {
			SDL_Log("Failed to load music: %s\n", SDL_GetError());
		}
		return music;
	}

	class AudioManager {
	public:
		AudioManager();
		~AudioManager();

		AudioManager(const AudioManager&) = delete;
		void operator=(const AudioManager&) = delete;

		void Init();
		
		Mix_MusicFinishedCallback OnMusicFinished();

		void UpdateTrackPos();

		int& GetVolume() { return volume; }
		int GetCurrentTrackNum() { return currentTrackNum; }
		Track GetCurrentTrack() { 
			if (queue.size() != 0) {
				return queue[currentTrackNum];
			}
			Track dummy;
			return dummy;
			
		}
		int& GetCurrentTrackPos() { return currentTrackPos; }
		std::vector<Track> GetQueue() { return queue; }

		bool IsMusicPaused() { return static_cast<bool>(Mix_PausedMusic()); }
		bool IsMusicPlaying() { return static_cast<bool>(Mix_PlayingMusic()); }

		void ChangeVolume();
		void ChangeMusicPos();
		void ChangeMusic();
		
		void PauseMusic();
		void ResumeMusic();

		void NextTrack();
		void PrevTrack();

		void LoadMusic();
	private:
		int audioOpen = 0;
		SDL_AudioSpec spec;
		int volume = 0;
		int currentTrackNum = 0;
		int currentTrackPos = 0;
		std::vector<Track> queue;
		Mix_Music* music = nullptr;

		std::vector<char> musicBuffer;
	};
}