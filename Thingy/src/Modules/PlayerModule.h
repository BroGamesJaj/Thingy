#pragma once
#include "Core/Module.h"

#include "Core\Managers\AudioManager.h"
#include "Core\Managers\NetworkManager.h"
#include "Core\Managers\ImageManager.h"
#include "Core\Managers\SceneManager.h"
#include "Core\Managers\MessageManager.h"

namespace Thingy {
	class PlayerModule : public Module {
	public:
		PlayerModule(MessageManager& messageManager, AudioManager& audioManager, ImageManager& imageManager) : m_MessageManager(messageManager), m_AudioManager(audioManager), m_ImageManager(imageManager), m_CurrentTime(audioManager.GetCurrentTrackPos()), m_AudioVolume(audioManager.GetVolume()), queue(audioManager.GetQueue()) {
			currentTrack = Track();
		}

		void SetupSubscriptions() override;
		void OnLoad(const std::variant<int, std::string> moduleState) override;
		void OnUpdate() override;	
		void Window() override;
		uint16_t OnRender() override;


		int DefaultWidth() const override { return 400; }


		MODULE_CLASS_NAME("PlayerModule")
	private:
		void QueueView();
		void PlayerView();

		uint16_t upProps = 0;

		MessageManager& m_MessageManager;
		AudioManager& m_AudioManager;
		ImageManager& m_ImageManager;

		bool open = true;
		bool isQueueOpen = false;

		std::vector<Track>& queue;
		std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>> queueTextures;

		Track currentTrack;

		int& m_CurrentTime;
		int& m_AudioVolume;

		bool changed = false;
	};
}