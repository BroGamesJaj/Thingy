#pragma once
#include "Core\Module.h"

#include "Core\Managers\AudioManager.h"
#include "Core\Managers\NetworkManager.h"
#include "Core\Managers\ImageManager.h"
#include "Core\Managers\SceneManager.h"
#include "Core\Managers\MessageManager.h"

namespace Thingy {
	class AlbumModule : public Module {
	public:
		AlbumModule(std::unique_ptr<MessageManager>& messageManager, std::unique_ptr<AudioManager>& audioManager, std::unique_ptr<ImageManager>& imageManager) : m_MessageManager(messageManager), m_AudioManager(audioManager), m_ImageManager(imageManager) {
		}
		void SetupSubscriptions() override;
		void OnLoad(const std::variant<int, std::string> moduleState) override;
		void OnUpdate() override;
		void Window() override;
		uint16_t OnRender() override;

		int DefaultWidth() const override { return 1280; }

		MODULE_CLASS_NAME("albumModule")
	private:

		struct SDL_TDeleter { void operator()(SDL_Texture* p) { SDL_DestroyTexture(p); } };

		uint16_t upProps = 0;

		int curr = 0;
		std::vector<Album> album;
		std::unique_ptr<MessageManager>& m_MessageManager;
		std::unique_ptr<AudioManager>& m_AudioManager;
		std::unique_ptr<ImageManager>& m_ImageManager;
		std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>> textures;

	};
}