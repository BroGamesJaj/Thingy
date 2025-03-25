#pragma once
#include "Core\Module.h"
#include <Core\HelperFunctions.h>

#include "Core\Managers\AudioManager.h"
#include "Core\Managers\NetworkManager.h"
#include "Core\Managers\ImageManager.h"
#include "Core\Managers\MessageManager.h"
#include "Core\Managers\AuthManager.h"

namespace Thingy {
	class PlaylistModule : public Module {
	public:
		PlaylistModule(MessageManager& messageManager, AudioManager& audioManager, ImageManager& imageManager, NetworkManager& networkManager, AuthManager& authManager) : m_MessageManager(messageManager), m_AudioManager(audioManager), m_ImageManager(imageManager), m_NetworkManager(networkManager), m_AuthManager(authManager), user(authManager.GetUser()) {}
		void SetupSubscriptions() override;
		void OnLoad(const std::variant<int, std::string> moduleState) override;
		void OnUpdate() override;
		void Window() override;
		uint16_t OnRender() override;


		const int DefaultSize() const override { 
			if (loggedIn && !m_AudioManager.GetQueue().empty()) {
				return 5;
			} else {
				return 6;
			}
		}

		MODULE_CLASS_NAME("playlistModule")
	private:
		void EditPlaylistModal();
		void DeletePlaylistModal();
		
		uint16_t upProps = 0;
		bool loggedIn = false;
		const User& user;

		int curr = 0;
		std::vector<Playlist> playlists;
		std::unordered_map<int, Track> tracks;
		std::vector<int> length;
		MessageManager& m_MessageManager;
		NetworkManager& m_NetworkManager;
		AudioManager& m_AudioManager;
		ImageManager& m_ImageManager;
		AuthManager& m_AuthManager;
		std::unique_ptr<SDL_Texture, SDL_TDeleter> playlistCover;
		std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>> textures;

		std::string editedPlaylistName = "";
		std::string editedDescription = "";
		bool editedIsPrivate = false;
		std::string editedCoverPath = "";
		std::unique_ptr<SDL_Texture, SDL_TDeleter> editedPlaylistCover;
		std::string error;
	};
}