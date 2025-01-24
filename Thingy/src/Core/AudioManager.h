#pragma once
#include "tpch.h"
#include "Track.h"
#include <SDL3_mixer\SDL_mixer.h>

namespace Thingy {
	class AudioManager {
	public:
		static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
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
		
		bool IsMusicPaused() { return Mix_PausedMusic; }
		bool IsMusicPlaying() { return Mix_PlayingMusic; }

		void ChangeVolume();
		void ChangeMusicPos();
		void ChangeMusic();
		
		void PauseMusic();
		void StartMusic();

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
		Mix_Music* music;

		bool DownloadFile(const std::string& url, std::vector<char>& buffer);
		Mix_Music* LoadMusicFromMemory(const std::vector<char>& buffer);

	};
}