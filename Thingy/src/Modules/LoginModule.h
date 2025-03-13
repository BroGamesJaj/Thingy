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
		LoginModule(MessageManager& messageManager, NetworkManager& networkManager, AuthManager& authManager) : m_MessageManager(messageManager), m_NetworkManager(networkManager), m_AuthManager(authManager) {};

		void SetupSubscriptions() override;
		void OnLoad(const std::variant<int, std::string> moduleState) override;
		void OnUpdate() override;
		void Window() override;
		uint16_t OnRender() override;

		int DefaultWidth() const override { return 1280; }

		MODULE_CLASS_NAME("loginModule")
	private:
		uint16_t upProps = 0;

		MessageManager& m_MessageManager;
		NetworkManager& m_NetworkManager;
		AuthManager& m_AuthManager;
		std::string email = "";
		std::string username = "";
		std::string password = "";
		std::string error = "";
		bool reg = false;

	};
}
