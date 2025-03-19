#pragma once
#include "Core\Module.h"
#include "Core\HelperFunctions.h"

#include "Core\Managers\MessageManager.h"
#include "Core\Managers\NetworkManager.h"
#include "Core\Managers\ImageManager.h"
#include "Core\Managers\AudioManager.h"
#include "Core\Managers\AuthManager.h"

namespace Thingy {
	class SearchModule : public Module {
	public:
		SearchModule(MessageManager& messageManager, NetworkManager& networkManager, ImageManager& imageManager, AudioManager& audioManager, AuthManager& authManager) : m_MessageManager(messageManager), m_NetworkManager(networkManager), m_ImageManager(imageManager), m_AudioManager(audioManager), m_AuthManager(authManager), user(m_AuthManager.GetUser()) {};

		void SetupSubscriptions() override;
		void OnLoad(const std::variant<int, std::string> moduleState) override;
		void OnUpdate() override;
		void Window() override;

		uint16_t OnRender() override;


		const int DefaultSize() const override {
			if (loggedIn && !m_AudioManager.GetQueue().empty()) {
				return 6;
			} else {
				return 7;
			}
		}
		MODULE_CLASS_NAME("searchModule")
	private:
		void LoadTextures();

		void AllResultsDisplay();
		void TrackResultsDisplay();
		void AlbumResultsDisplay();
		void ArtistResultsDisplay();
		void PlaylistResultsDisplay();

		void GetTrackResults(std::string term);
		void GetAlbumResults(std::string term);
		void GetArtistResults(std::string term);
		void GetPlaylistResults(std::string term);

		uint16_t upProps = 0;
		bool loggedIn = false;
		
		MessageManager& m_MessageManager;
		NetworkManager& m_NetworkManager;
		ImageManager& m_ImageManager;
		AudioManager& m_AudioManager;
		AuthManager& m_AuthManager;
		
		const User& user;

		std::unordered_map<uint32_t, std::future<Image>> imageFutures;
		std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>> textures;
		std::vector<std::string> buttons = { "all", "tags", "tracks", "albums", "artists", "playlists" };
		int whichToggled = 0;

		std::vector<Track> trackResults;
		std::vector<Album> albumResults;
		std::vector<Artist> artistResults;
		std::vector<Playlist> playlistResults;

	};
}