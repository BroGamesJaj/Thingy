#pragma once
#include "Core\Module.h"
#include "Core\Track.h"
#include "Core\Album.h"
#include "Core\Artist.h"
#include "Core\NetworkManager.h"
#include "Core\AudioManager.h"
#include "Core\ImageManager.h"
#include "Core\Image.h"

namespace Thingy {
	class PopularsModule : public Module {
	public:

		PopularsModule(std::unique_ptr<NetworkManager>& networkManager, std::unique_ptr<AudioManager>& audioManager, std::unique_ptr<ImageManager>& imageManager, SDL_Renderer* renderer) : m_NetworkManager(networkManager), m_AudioManager(audioManager), m_ImageManager(imageManager), m_Renderer(renderer){
		};
		void OnLoad() override;
		void OnUpdate() override;
		void Window() override;

		uint16_t OnRender() override;


		int MinWidth() const override { return 500; }
		int MaxWidth() const override { return 1280; }

		void GetPopulars();
		


		MODULE_CLASS_NAME("PopularsModule")

	private:
		struct SDL_TDeleter { void operator()(SDL_Texture* p) { SDL_DestroyTexture(p); } };
		
		uint16_t upProps = 0;

		SDL_Renderer* m_Renderer = nullptr;
		std::unique_ptr<NetworkManager>& m_NetworkManager;
		std::unique_ptr<AudioManager>& m_AudioManager;
		std::unique_ptr<ImageManager>& m_ImageManager;
		std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>> textures;
		std::vector<Track> weeklyTracks;
		std::vector<Track> monthlyTracks;
		std::vector<Album> weeklyAlbums; 
		std::vector<Album> monthlyAlbums;
		std::vector<Artist> weeklyArtists; 
		std::vector<Artist> monthlyArtists;
	};
}