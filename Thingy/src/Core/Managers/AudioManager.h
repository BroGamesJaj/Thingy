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
		AudioManager(std::vector<uint8_t>& buffer, NetworkManager& networkManager, MessageManager& messageManager);
		~AudioManager();

		AudioManager(const AudioManager&) = delete;
		void operator=(const AudioManager&) = delete;
		
		void CleanUp();

		void UpdateTrackPos();

		int& GetVolume() noexcept { return volume; }
		
		Track GetCurrentTrack() const { 
			if (!queue.empty()) {
				return *current;
			}
			Track dummy;
			dummy.id = -1;
			dummy.title = "dummy";
			dummy.artistName = "dummy Artist";
			dummy.duration = 10;
			return dummy;
			
		}
		int& GetCurrentTrackPos() noexcept { return currentTrackPos; }
		const int GetCurrentTrackDuration() noexcept { return music ? Mix_MusicDuration(music) : 0; }
		std::list<Track>& GetQueue() noexcept { return queue; }
 
		bool IsMusicPaused() noexcept { return {Mix_PausedMusic()}; }
		bool IsMusicPlaying() noexcept { return {Mix_PlayingMusic()}; }
		bool IsMusicLoaded() noexcept { return music ? true : false; }

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
		void ChangeMusicByTrack(const int trackID);
		void ShuffleQueue();

		void ClearQueue() noexcept {
			queue.clear();
		}
		void ClearHistory() noexcept {
			history.clear();
		}

	private:
		NetworkManager& m_NetworkManager;
		MessageManager& m_MessageManager;

		int audioOpen = 0;
		SDL_AudioSpec spec;
		int volume = 0;
		int currentTrackPos = 0;
		std::list<Track> queue;
		std::list<Track> history;
		std::list<Track> originalQueue;
		std::list<Track>::iterator current = queue.end();
		bool shuffled = false;

		Mix_Music* music = nullptr;

		std::vector<uint8_t>& musicBuffer;
	};
}