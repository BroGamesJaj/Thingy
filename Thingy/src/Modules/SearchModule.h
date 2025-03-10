#pragma once
#include "Core\Module.h"

#include "Core\Managers\MessageManager.h"
#include "Core\Managers\NetworkManager.h"
#include "Core\Managers\ImageManager.h"

namespace Thingy {
	class SearchModule : Module {
	public:
		SearchModule(std::unique_ptr<MessageManager>& messageManager, std::unique_ptr<NetworkManager>& networkManager, std::unique_ptr<ImageManager>& imageManager) : m_MessageManager(messageManager), m_NetworkManager(networkManager), m_ImageManager(imageManager) {};

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
		std::unique_ptr<ImageManager>& m_ImageManager;

		uint16_t upProps = 0;

		std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>> textures;
	};
}