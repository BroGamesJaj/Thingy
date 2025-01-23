#pragma once
#include "tpch.h"
#include "Artist.h"

namespace Thingy {
	class Track {
		Track(int id = 0, std::string title = "", int duration = 0, std::string image = "", int artistID = 0, std::string artistName = "", int albumID = 0,
			std::string albumName = "", std::string albumImageURL = "", std::string releaseDate = "", std::string audioURL = "")
			: trackID(id), trackTitle(title), trackDuration(duration), trackImageURL(image),
			m_ArtistID(artistID), m_ArtistName(artistName), m_AlbumID(albumID), m_AlbumName(albumName),
			m_AlbumImageURL(albumImageURL), trackReleaseDate(releaseDate), trackAudioURL(audioURL)
		{}
	private:
		int trackID;
		std::string trackTitle;
		int trackDuration;
		std::string trackImageURL;
		int m_ArtistID;
		std::string m_ArtistName;
		int m_AlbumID;
		std::string m_AlbumName;
		std::string m_AlbumImageURL;
		std::string trackReleaseDate;
		std::string trackAudioURL;
	};

	
}