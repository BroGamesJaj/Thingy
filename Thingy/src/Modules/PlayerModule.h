#pragma once
#include "Core/Module.h"
#include "Core/AudioManager.h"
#include "Core/ImageManager.h"

namespace Thingy {
	class PlayerModule : public Module {
	public:
		PlayerModule(std::unique_ptr<AudioManager>& audioManager, std::unique_ptr<ImageManager>& imageManager) : m_AudioManager(audioManager), m_ImageManager(imageManager), m_CurrentTime(audioManager->GetCurrentTrackPos()), m_AudioVolume(audioManager->GetVolume()) {
			m_TrackID = 0;
			m_TrackName = "";
			m_TrackArtist = "";
			m_TrackDuration = 0;
		}

		void OnLoad() override;
		void OnUpdate() override;	
		void Window() override;
		uint16_t OnRender() override;

		int MinWidth() const override { return 350; }
		int MaxWidth() const override { return 400; }

		void SetCurrentTrack( std::string trackName, std::string trackArtist, int trackDuration) {
			m_TrackName = trackName;
			m_TrackArtist = trackArtist;
			m_TrackDuration = trackDuration;
		}



		MODULE_CLASS_NAME("PlayerModule")
	private:

		struct SDL_TDeleter { void operator()(SDL_Texture* p) { SDL_DestroyTexture(p); } };

		uint16_t upProps = 0;

		std::unique_ptr<AudioManager>& m_AudioManager;
		std::unique_ptr<ImageManager>& m_ImageManager;

		int m_TrackID;
		std::string m_TrackName;
		std::string m_TrackArtist;
		std::unique_ptr<SDL_Texture, SDL_TDeleter> image;
		int m_TrackDuration;

		int& m_CurrentTime;
		int& m_AudioVolume;

		bool changed = false;
	};
}