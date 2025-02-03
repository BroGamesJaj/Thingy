#pragma once
#include "tpch.h"
#include "Track.h"
#include "NetworkManager.h"
#include <SDL3\SDL.h>
#include <SDL3_mixer\SDL_mixer.h>

namespace Thingy {

	

	
	class AudioManager {
	public:
		AudioManager(std::vector<uint8_t>& buffer, std::unique_ptr<NetworkManager>& networkManager);
		~AudioManager();

		AudioManager(const AudioManager&) = delete;
		void operator=(const AudioManager&) = delete;

		void Init();
		
		void CleanUp();

		void UpdateTrackPos();

		int& GetVolume() { return volume; }
		int GetCurrentTrackNum() const { return currentTrackNum; }
		Track GetCurrentTrack() { 
			if (queue.size() != 0) {
				return queue[currentTrackNum];
			}
			Track dummy;
			return dummy;
			
		}
		int& GetCurrentTrackPos() { return currentTrackPos; }
		int GetCurrentTrackDuration() { return music ? Mix_MusicDuration(music) : 0; }
		std::vector<Track> GetQueue() { return queue; }

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

		Mix_Music* LoadMusicFromMemory(const std::vector<uint8_t>& buffer) {
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

		void LoadMusicFromTrack(Track& track) {
			Mix_HaltMusic();
			queue.clear();
			queue.push_back(track);
			m_NetworkManager->DownloadAudio(track.audioURL, musicBuffer);
		}


	private:
		std::unique_ptr<NetworkManager>& m_NetworkManager;

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