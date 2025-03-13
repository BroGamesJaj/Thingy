#pragma once
#include "Core\Module.h"

#include "Core\Managers\MessageManager.h"
#include "Core\Managers\NetworkManager.h"
#include "Core\Managers\ImageManager.h"

namespace Thingy {
	class SearchModule : Module {
	public:
		SearchModule(MessageManager& messageManager, NetworkManager& networkManager, ImageManager& imageManager) : m_MessageManager(messageManager), m_NetworkManager(networkManager), m_ImageManager(imageManager) {};

		void SetupSubscriptions() override;
		void OnLoad(const std::variant<int, std::string> moduleState) override;
		void OnUpdate() override;
		void Window() override;

		uint16_t OnRender() override;


		int DefaultWidth() const override { return 1280; }

		MODULE_CLASS_NAME("SearchModule")
	private:
		
		MessageManager& m_MessageManager;
		NetworkManager& m_NetworkManager;
		ImageManager& m_ImageManager;

		uint16_t upProps = 0;

		std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>> textures;
	};
}