#pragma once
#include "Core\Module.h"
#include <Core\HelperFunctions.h>

#include "Core\Managers\AudioManager.h"
#include "Core\Managers\NetworkManager.h"
#include "Core\Managers\ImageManager.h"
#include "Core\Managers\MessageManager.h"

namespace Thingy {
	class ArtistModule : public Module {
	public:
		ArtistModule(std::unique_ptr<MessageManager>& messageManager, std::unique_ptr<AudioManager>& audioManager, std::unique_ptr<ImageManager>& imageManager, std::unique_ptr<NetworkManager>& networkManager) : m_MessageManager(messageManager), m_AudioManager(audioManager), m_ImageManager(imageManager), m_NetworkManager(networkManager) {}
		void SetupSubscriptions() override;
		void OnLoad(const std::variant<int, std::string> moduleState) override;
		void OnUpdate() override;
		void Window() override;
		uint16_t OnRender() override;

		int DefaultWidth() const override { return 1280; }

		MODULE_CLASS_NAME("artistModule")
	private:
		uint16_t upProps = 0;

		int curr = 0;
		std::vector<Artist> artists;
		std::unordered_map<uint32_t, std::vector<Album>> albums;
		std::unique_ptr<MessageManager>& m_MessageManager;
		std::unique_ptr<NetworkManager>& m_NetworkManager;
		std::unique_ptr<AudioManager>& m_AudioManager;
		std::unique_ptr<ImageManager>& m_ImageManager;
		std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>> textures;
		std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>>> albumTextures;

	};
}