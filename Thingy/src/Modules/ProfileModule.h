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
		ProfileModule(MessageManager& messageManager, ImageManager& imageManager, NetworkManager& networkManager, AuthManager& authManager) : m_MessageManager(messageManager), m_ImageManager(imageManager), m_NetworkManager(networkManager), m_AuthManager(authManager), user(authManager.GetUser()) {}
		void SetupSubscriptions() override;
		void OnLoad(const std::variant<int, std::string> moduleState) override;
		void OnUpdate() override;
		void Window() override;
		uint16_t OnRender() override;

		const int DefaultSize() const override { return 1; }

		MODULE_CLASS_NAME("profileModule")
	private:
		void UserInfoChanged();

		uint16_t upProps = 0;

		MessageManager& m_MessageManager;
		NetworkManager& m_NetworkManager;
		ImageManager& m_ImageManager;
		AuthManager& m_AuthManager;

		bool loggedIn = false;
		bool editingDesc = false;
		bool showDescChangePopup = false;
		std::string newDescription = "";

		const User& user;

	};
}