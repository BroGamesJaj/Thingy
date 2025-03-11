#pragma once
#include <regex>
#include "Core\Module.h"
#include "Core\HelperFunctions.h"

#include "Core\Managers\MessageManager.h"
#include "Core\Managers\NetworkManager.h"
#include "Core\Managers\AuthManager.h"

namespace Thingy {
	class LoginModule : public Module {
	public:
		LoginModule(std::unique_ptr<MessageManager>& messageManager, std::unique_ptr<NetworkManager>& networkManager, std::unique_ptr<AuthManager>& authManager) : m_MessageManager(messageManager), m_NetworkManager(networkManager), m_AuthManager(authManager) {};

		void SetupSubscriptions() override;
		void OnLoad(const std::variant<int, std::string> moduleState) override;
		void OnUpdate() override;
		void Window() override;
		uint16_t OnRender() override;

		int DefaultWidth() const override { return 1280; }

		MODULE_CLASS_NAME("loginModule")
	private:

		std::unique_ptr<MessageManager>& m_MessageManager;
		std::unique_ptr<NetworkManager>& m_NetworkManager;
		std::unique_ptr<AuthManager>& m_AuthManager;
		std::string email = "";
		std::string username = "";
		std::string password = "";
		std::string error = "";
		bool reg = false;

	};
}
