#pragma once
#include "Core\Module.h"
#include <Core\HelperFunctions.h>

#include "Core\Managers\ImageManager.h"
#include "Core\Managers\MessageManager.h"
#include "Core\Managers\AuthManager.h"

namespace Thingy {
	class PlaylistListModule : public Module {
	public:
		PlaylistListModule(MessageManager& messageManager, ImageManager& imageManager, NetworkManager& networkManager, AuthManager& authManager) : m_MessageManager(messageManager), m_ImageManager(imageManager), m_NetworkManager(networkManager), m_AuthManager(authManager), user(authManager.GetUser()) {}
		void SetupSubscriptions() override;
		void OnLoad(const std::variant<int, std::string> moduleState) override;
		void OnUpdate() override;
		void Window() override;
		uint16_t OnRender() override;

		const int DefaultSize() const override { return 2; }

		MODULE_CLASS_NAME("PlaylistListModule")
	private:
		void UpdateUserInfo();
		void NewPlaylistModal();

		uint16_t upProps = 0;

		MessageManager& m_MessageManager;
		ImageManager& m_ImageManager;
		NetworkManager& m_NetworkManager;
		AuthManager& m_AuthManager;
		
		const User& user;
		bool loggedIn = false;

		std::string newPlaylistName = "";
		std::string newDescription = "";
		bool isPrivate = false;
		std::string coverPath = "";
		std::unique_ptr<SDL_Texture, SDL_TDeleter> newPlaylistCover = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetDefaultPlaylistImage());
		std::string error = "";
	};
}