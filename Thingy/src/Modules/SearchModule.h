#pragma once
#include "Core\Module.h"

#include "Core\Managers\MessageManager.h"
#include "Core\Managers\NetworkManager.h"

namespace Thingy {
	class SearchModule : Module {
	public:
		SearchModule(std::unique_ptr<MessageManager>& messageManager, std::unique_ptr<NetworkManager>& networkManager) : m_MessageManager(messageManager), m_NetworkManager(networkManager) {};

		void SetupSubscriptions() override;
		void OnLoad(const std::variant<int, std::string> moduleState) override;
		void OnUpdate() override;
		void Window() override;

		uint16_t OnRender() override;


		int DefaultWidth() const override { return 1280; }

		MODULE_CLASS_NAME("SearchModule")
	private:
		std::unique_ptr<MessageManager>& m_MessageManager;
		std::unique_ptr<NetworkManager>& m_NetworkManager;

	};
}