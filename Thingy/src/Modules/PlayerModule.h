#pragma once
#include "Core/Module.h"
#include "Core/Track.h"
namespace Thingy {
	class PlayerModule : Module {
	public:
		PlayerModule(int& currentTime, int& volume) : m_CurrentTime(currentTime), m_AudioVolume(volume) {
			m_TrackImageURL = "";
			m_TrackName = "";
			m_TrackArtist = "";
			m_TrackDuration = 0;
		}

		void OnUpdate() override;

		void OnRender() override;

		void SetCurrentTrack(std::string trackImageURL, std::string trackName, std::string trackArtist, int trackDuration) {
			m_TrackImageURL = trackImageURL;
			m_TrackName = trackName;
			m_TrackArtist = trackArtist;
			m_TrackDuration = trackDuration;
		}

		MODULE_CLASS_NAME("PlayerModule")
	private:
		std::string m_TrackImageURL;
		std::string m_TrackName;
		std::string m_TrackArtist;
		int m_TrackDuration;

		int& m_CurrentTime;
		int& m_AudioVolume;
	};
}