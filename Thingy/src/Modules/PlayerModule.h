#pragma once
#include "Core/Module.h"
#include "Core/AudioManager.h"

namespace Thingy {
	class PlayerModule : public Module {
	public:
		PlayerModule(std::unique_ptr<AudioManager>& audioManager) : m_AudioManager(audioManager), m_CurrentTime(audioManager->GetCurrentTrackPos()), m_AudioVolume(audioManager->GetVolume()) {
			m_TrackImageURL = "";
			m_TrackName = "";
			m_TrackArtist = "";
			m_TrackDuration = 0;
		}

		void OnUpdate() override;
			
		void Window(std::string title) override;

		void OnRender() override;

		int MinWidth() const override { return 350; }
		int MaxWidth() const override { return 400; }

		void SetCurrentTrack(std::string trackImageURL, std::string trackName, std::string trackArtist, int trackDuration) {
			m_TrackImageURL = trackImageURL;
			m_TrackName = trackName;
			m_TrackArtist = trackArtist;
			m_TrackDuration = trackDuration;
		}



		MODULE_CLASS_NAME("PlayerModule")
	private:
		std::unique_ptr<AudioManager>& m_AudioManager;

		std::string m_TrackImageURL;
		std::string m_TrackName;
		std::string m_TrackArtist;
		int m_TrackDuration;

		int& m_CurrentTime;
		int& m_AudioVolume;
	};
}