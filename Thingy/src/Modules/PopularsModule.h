#pragma once
#include "Core\Module.h"
#include "Core\Track.h"
#include "Core\Album.h"
#include "Core\Artist.h"

namespace Thingy {
	class PopularsModule : Module {
	public:


		void OnUpdate() override;

		void Window(std::string title) override;

		void OnRender() override;

		

		MODULE_CLASS_NAME("PopularsModule")

	private:
		std::vector<Track> weeklyTracks;
		std::vector<Track> monthlyTracks;
		std::vector<Album> weeklyAlbums; 
		std::vector<Album> monthlyAlbums;
		std::vector<Artist> weeklyArtists; 
		std::vector<Artist> monthlyArtists;
	};
}