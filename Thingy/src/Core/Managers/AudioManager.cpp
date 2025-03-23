#include "tpch.h"
#include "AudioManager.h"

namespace Thingy {

	AudioManager::AudioManager(std::vector<uint8_t>& buffer, NetworkManager& networkManager, MessageManager& messageManager) : musicBuffer(buffer), m_NetworkManager(networkManager), m_MessageManager(messageManager) {
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

		m_MessageManager.Subscribe("startMusic", "audioManager", [this](MessageData data) {
			if (data.type() == typeid(Track)) {
				Track track = std::any_cast<Track>(data);
				LoadMusicFromTrack(track);
				ChangeMusic();
				ResumeMusic();
			} else {
				PlayQueueFromStart();
			}
			});

		m_MessageManager.Subscribe("addToQueue", "audioManager", [this](MessageData data) {
			if (data.type() == typeid(std::vector<Track>)) {
				std::vector<Track> tracks = std::any_cast<std::vector<Track>>(data);
				AddToQueue(tracks);
			} else if (data.type() == typeid(Track)) {
				Track track = std::any_cast<Track>(data);
				queue.push_back(track);
			}
			shuffled = false;
			});

		m_MessageManager.Subscribe("newHistory", "audioManager", [this](const MessageData data) {
			history.clear();
			if (data.type() == typeid(std::vector<Track>)) {
				std::vector<Track> tracks = std::any_cast<std::vector<Track>>(data);
				history.insert(history.end(), tracks.begin(), tracks.end());
			} else if (data.type() == typeid(Track)) {
				Track track = std::any_cast<Track>(data);
				history.push_back(track);
			}
			});

		m_MessageManager.Subscribe("logout", "audioManager", [this](const MessageData data) {
			Mix_HaltMusic();
			history.clear();
			queue.clear();
			shuffled = false;
			current = queue.begin();
			});
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

	void AudioManager::AudioLoop() {
		if (IsMusicLoaded()) UpdateTrackPos();
		if (empty && !queue.empty()) {
			empty = false;
			m_MessageManager.Publish("queueChanged", "");
		} else if (!empty && queue.empty()) {
			empty = true;
			m_MessageManager.Publish("queueChanged", "");
		}
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
		if (current != queue.end()) {
			history.push_back(*current);
			++current;
			if (current == queue.end() && !history.empty()) {
				queue = history;
				current = queue.begin();
				LoadMusicFromQueue();
				ChangeMusic();
			} else if (current == queue.end()) {
				--current;
				Mix_PlayMusic(music, 0);
				PauseMusic();
				
			} else {
				queue.pop_front();
				LoadMusicFromQueue();
				ChangeMusic();
				ResumeMusic();
			};
		}
	}

	void AudioManager::PrevTrack() {
		if (!history.empty()) {
			queue.push_front(history.back());
			current = queue.begin();
			history.pop_back();
			LoadMusicFromQueue();
			ChangeMusic();
			ResumeMusic();
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

	Mix_Music* AudioManager::LoadMusicFromMemory(const std::vector<uint8_t>& buffer) {
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

	void AudioManager::LoadMusicFromTrack(Track& track) {
		Mix_HaltMusic();
		queue.clear();
		queue.push_back(track);
		current = queue.begin();
		m_NetworkManager.DownloadAudio(track.audioURL, musicBuffer);
	}

	void AudioManager::LoadMusicFromQueue() {
		Mix_HaltMusic();
		m_NetworkManager.DownloadAudio(current->audioURL, musicBuffer);
	}

	void AudioManager::AddToQueue(const std::vector<Track>& tracks) {
		queue.insert(queue.end(), tracks.begin(), tracks.end());
	}

	void AudioManager::PlayQueueFromStart() {
		if (!queue.empty()) {
			current = queue.begin();
			LoadMusicFromQueue();
			ChangeMusic();
			ResumeMusic();
		}
	}

	void AudioManager::ChangeMusicByTrack(const int trackID) {
		auto it = std::find_if(queue.begin(), queue.end(), [&](const Track& track) {
			return track.id == trackID;
			});
		if (it != queue.end()) {
			history.push_back(*current);
			queue.erase(queue.begin(), it);
			current = queue.begin();
			LoadMusicFromQueue();
			ChangeMusic();
			ResumeMusic();
		}
	}

	void AudioManager::ShuffleQueue() {
		if (shuffled) {
			if (current != queue.end()) {
				auto it = std::find_if(originalQueue.begin(), originalQueue.end(), [&](const Track& track) {
					return track.id == current->id;
					});
				if (it != originalQueue.end()) {
					queue.assign(it, originalQueue.end());
					current = queue.begin();
				}
				current = queue.begin();
				shuffled = false;
			}
		} else {
			if (queue.empty() || queue.size() < 2) return;
			originalQueue = queue;
			std::vector<Track> temp(queue.begin(), queue.end());
			std::shuffle(temp.begin()+1, temp.end(), std::random_device());

			queue.assign(temp.begin(), temp.end());
			current = queue.begin();
			shuffled = true;
		}
	}


}
