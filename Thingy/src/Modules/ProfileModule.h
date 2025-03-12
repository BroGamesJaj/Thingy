#pragma once
#include "Core\Module.h"
#include "Core\HelperFunctions.h"

#include "Core\Managers\NetworkManager.h"
#include "Core\Managers\ImageManager.h"
#include "Core\Managers\MessageManager.h"
#include "Core\Managers\AuthManager.h"

namespace Thingy {
	class ProfileModule : public Module {
	public:
		ProfileModule(std::unique_ptr<MessageManager>& messageManager, std::unique_ptr<ImageManager>& imageManager, std::unique_ptr<NetworkManager>& networkManager, std::unique_ptr<AuthManager>& authManager) : m_MessageManager(messageManager), m_ImageManager(imageManager), m_NetworkManager(networkManager), m_AuthManager(authManager), user(authManager->GetUser()) {}
		void SetupSubscriptions() override;
		void OnLoad(const std::variant<int, std::string> moduleState) override;
		void OnUpdate() override;
		void Window() override;
		uint16_t OnRender() override;


		int DefaultWidth() const override { return 1280; }

		MODULE_CLASS_NAME("profileModule")
	private:
		void UserInfoChanged();

		uint16_t upProps = 0;

		std::unique_ptr<MessageManager>& m_MessageManager;
		std::unique_ptr<NetworkManager>& m_NetworkManager;
		std::unique_ptr<ImageManager>& m_ImageManager;
		std::unique_ptr<AuthManager>& m_AuthManager;

		bool loggedIn = false;
		bool editingDesc = false;
		bool showDescChangePopup = false;
		std::string newDescription = "";

		const User& user;
		std::unique_ptr<SDL_Texture, SDL_TDeleter> pfp;
		std::unordered_map<int, std::unique_ptr<SDL_Texture, SDL_TDeleter>> playlistCovers;

	};
}