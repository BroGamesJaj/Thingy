#pragma once
#include "Core\Module.h"

#include "Core\Managers\AudioManager.h"
#include "Core\Managers\NetworkManager.h"
#include "Core\Managers\ImageManager.h"
#include "Core\Managers\SceneManager.h"
#include "Core\Managers\MessageManager.h"

#include "Core\HelperFunctions.h"

namespace Thingy {
	class PopularsModule : public Module {
	public:

		PopularsModule(MessageManager& messageManager, NetworkManager& networkManager, AudioManager& audioManager, ImageManager& imageManager, SDL_Renderer* renderer) : m_MessageManager(messageManager), m_NetworkManager(networkManager), m_AudioManager(audioManager), m_ImageManager(imageManager), m_Renderer(renderer){
		};

		void SetupSubscriptions() override;
		void OnLoad(const std::variant<int, std::string> moduleState) override;
		void OnUpdate() override;
		void Window() override;


		uint16_t OnRender() override;

		const int DefaultSize() const override { 
			if (loggedIn && !m_AudioManager.GetQueue().empty()) {
				return 5;
			} else if (loggedIn) {
				return 8;
			} else {
				return 6;
			}
		}

		void GetPopulars();

		MODULE_CLASS_NAME("PopularsModule")

	private:
		uint16_t upProps = 0;

		SDL_Renderer* m_Renderer = nullptr;

		MessageManager& m_MessageManager;
		NetworkManager& m_NetworkManager;
		AudioManager& m_AudioManager;
		ImageManager& m_ImageManager;
		std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>> textures;
		std::vector<Track> weeklyTracks;
		std::vector<Track> monthlyTracks;
		std::vector<Album> weeklyAlbums; 
		std::vector<Album> monthlyAlbums;
		std::vector<Artist> weeklyArtists; 
		std::vector<Artist> monthlyArtists;
		std::time_t lastFetch = 0;
		bool loggedIn = false;
	};
}