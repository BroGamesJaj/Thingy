#pragma once
#include "Core\Module.h"
#include <Core\HelperFunctions.h>

#include "Core\Managers\AudioManager.h"
#include "Core\Managers\NetworkManager.h"
#include "Core\Managers\ImageManager.h"
#include "Core\Managers\MessageManager.h"
#include "Core\Managers\AuthManager.h"

namespace Thingy {
	class AlbumModule : public Module {
	public:
		AlbumModule(MessageManager& messageManager, AudioManager& audioManager, ImageManager& imageManager, NetworkManager& networkManager, AuthManager& authManager) : m_MessageManager(messageManager), m_AudioManager(audioManager), m_ImageManager(imageManager), m_NetworkManager(networkManager), m_AuthManager(authManager) {
		}
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

		MODULE_CLASS_NAME("albumModule")
	private:
		void PlaylistModal();
		void UserInfoChanged();
		uint16_t upProps = 0;

		int curr = 0;
		std::vector<Album> album;
		int length = 0;
		MessageManager& m_MessageManager;
		NetworkManager& m_NetworkManager;
		AudioManager& m_AudioManager;
		ImageManager& m_ImageManager;
		AuthManager& m_AuthManager;
		std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>> textures;
		std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>> playlistTextures;

		const User& user = m_AuthManager.GetUser();
		bool loggedIn = false;
		std::unordered_map<uint32_t, bool> selectedPlaylists;
	};
}