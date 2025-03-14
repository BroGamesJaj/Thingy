#pragma once
#include "Core\Module.h"
#include <Core\HelperFunctions.h>

#include "Core\Managers\AudioManager.h"
#include "Core\Managers\NetworkManager.h"
#include "Core\Managers\ImageManager.h"
#include "Core\Managers\MessageManager.h"

namespace Thingy {
	class AlbumModule : public Module {
	public:
		AlbumModule(MessageManager& messageManager, AudioManager& audioManager, ImageManager& imageManager, NetworkManager& networkManager) : m_MessageManager(messageManager), m_AudioManager(audioManager), m_ImageManager(imageManager), m_NetworkManager(networkManager) {
		}
		void SetupSubscriptions() override;
		void OnLoad(const std::variant<int, std::string> moduleState) override;
		void OnUpdate() override;
		void Window() override;
		uint16_t OnRender() override;

		const int DefaultSize() const override { return 4; }

		MODULE_CLASS_NAME("albumModule")
	private:

		uint16_t upProps = 0;

		int curr = 0;
		std::vector<Album> album;
		int length = 0;
		MessageManager& m_MessageManager;
		NetworkManager& m_NetworkManager;
		AudioManager& m_AudioManager;
		ImageManager& m_ImageManager;
		std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>> textures;

	};
}