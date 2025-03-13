#pragma once
#include "Core\Module.h"
#include <Core\HelperFunctions.h>

#include "Core\Managers\ImageManager.h"
#include "Core\Managers\MessageManager.h"
#include "Core\Managers\AuthManager.h"

namespace Thingy {
	class PlaylistListModule : public Module {
	public:
		PlaylistListModule(MessageManager& messageManager, ImageManager& imageManager, NetworkManager& networkManager, AuthManager& authManager) : m_MessageManager(messageManager), m_ImageManager(imageManager), m_AuthManager(authManager), user(authManager.GetUser()) {}
		void SetupSubscriptions() override;
		void OnLoad(const std::variant<int, std::string> moduleState) override;
		void OnUpdate() override;
		void Window() override;
		uint16_t OnRender() override;

		int DefaultWidth() const override { return 400; }

		MODULE_CLASS_NAME("playlistListModule")
	private:
		void UpdateUserInfo();

		uint16_t upProps = 0;

		MessageManager& m_MessageManager;
		ImageManager& m_ImageManager;
		AuthManager& m_AuthManager;
		const User& user;
		bool loggedIn = false;
		std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>> playlistTextures;

	};
}