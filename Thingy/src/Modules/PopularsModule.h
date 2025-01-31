#pragma once
#include "Core\Module.h"
#include "Core\Track.h"
#include "Core\Album.h"
#include "Core\Artist.h"
#include "Core\NetworkManager.h"
#include "Core\Image.h"

namespace Thingy {
	class PopularsModule : public Module {
	public:

		PopularsModule(std::unique_ptr<NetworkManager>& networkManager, SDL_Renderer* renderer) : m_NetworkManager(networkManager), m_Renderer(renderer){
		};
		void OnUpdate() override;

		void Window(std::string title) override;

		void OnRender() override;

		int MinWidth() const override { return 500; }
		int MaxWidth() const override { return 1280; }

		void GetPopulars();
		SDL_Texture* GetTexture(std::string& url);

		MODULE_CLASS_NAME("PopularsModule")

	private:
		struct SDL_TDeleter { void operator()(SDL_Texture* p) { SDL_DestroyTexture(p); } };
		
		SDL_Renderer* m_Renderer = nullptr;
		std::unique_ptr<NetworkManager>& m_NetworkManager;
		std::vector<Track> weeklyTracks;
		std::vector<Track> monthlyTracks;
		std::vector<Album> weeklyAlbums; 
		std::vector<Album> monthlyAlbums;
		std::vector<Artist> weeklyArtists; 
		std::vector<Artist> monthlyArtists;
	};
}