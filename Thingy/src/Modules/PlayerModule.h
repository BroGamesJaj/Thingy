#pragma once
#include "Core/Module.h"
#include "Core/Track.h"
namespace Thingy {
	class PlayerModule : Module {
	public:
		PlayerModule(std::string trackImageURL, std::string trackName, std::string trackArtist, int trackDuration, int& currentTime, int& volume) : m_TrackImageURL(trackImageURL), m_TrackName(trackName), m_TrackArtist(trackArtist), m_TrackDuration(trackDuration), m_CurrentTime(currentTime), m_AudioVolume(volume) {
		}

		void OnUpdate() override;

		void OnRender() override;

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