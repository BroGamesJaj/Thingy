#pragma once
#include "tpch.h"
#include "Core\Track.h"
#include "NetworkManager.h"
#include "MessageManager.h"

#include <SDL3\SDL.h>
#include <SDL3_mixer\SDL_mixer.h>

namespace Thingy {

	

	
	class AudioManager {
	public:
		AudioManager(std::vector<uint8_t>& buffer, std::unique_ptr<NetworkManager>& networkManager, std::unique_ptr<MessageManager>& messageManager);
		~AudioManager();

		AudioManager(const AudioManager&) = delete;
		void operator=(const AudioManager&) = delete;
		
		void CleanUp();

		void UpdateTrackPos();

		int& GetVolume() { return volume; }
		int GetCurrentTrackNum() const { return currentTrackNum; }
		Track GetCurrentTrack() const { 
			if (queue.size() != 0) {
				return queue[currentTrackNum];
			}
			Track dummy;
			dummy.id = -1;
			dummy.title = "dummy";
			dummy.artistName = "dummy Artist";
			dummy.duration = 10;
			return dummy;
			
		}
		int& GetCurrentTrackPos() { return currentTrackPos; }
		int GetCurrentTrackDuration() { return music ? Mix_MusicDuration(music) : 0; }
		std::vector<Track>& GetQueue() { return queue; }
	

		bool IsMusicPaused() { return static_cast<bool>(Mix_PausedMusic()); }
		bool IsMusicPlaying() { return static_cast<bool>(Mix_PlayingMusic()); }
		bool IsMusicLoaded() { return music ? true : false; }

		void ChangeVolume();
		void ChangeMusicPos();
		void ChangeMusic();

		void PauseMusic();
		void ResumeMusic();

		void NextTrack();
		void PrevTrack();

		void LoadMusic();

		Mix_Music* LoadMusicFromMemory(const std::vector<uint8_t>& buffer);

		void LoadMusicFromTrack(Track& track);
		void LoadMusicFromQueue();

		void AddToQueue(const std::vector<Track>& tracks);
		void PlayQueueFromStart();
		void ClearQueue() {
			queue.clear();
		}


	private:
		std::unique_ptr<NetworkManager>& m_NetworkManager;
		std::unique_ptr<MessageManager>& m_MessageManager;

		int audioOpen = 0;
		SDL_AudioSpec spec;
		int volume = 0;
		int currentTrackNum = 0;
		int currentTrackPos = 0;
		std::vector<Track> queue;
		Mix_Music* music = nullptr;

		std::vector<uint8_t>& musicBuffer;
	};
}