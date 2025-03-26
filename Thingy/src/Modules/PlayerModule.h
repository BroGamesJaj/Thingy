#pragma once
#include "Core\Module.h"
#include "Core\HelperFunctions.h"

#include "Core\Managers\AudioManager.h"
#include "Core\Managers\NetworkManager.h"
#include "Core\Managers\ImageManager.h"
#include "Core\Managers\SceneManager.h"
#include "Core\Managers\MessageManager.h"
#include "Core\Managers\AuthManager.h"

namespace Thingy {
	class PlayerModule : public Module {
	public:
		PlayerModule(MessageManager& messageManager, AudioManager& audioManager, ImageManager& imageManager, AuthManager& authManager, NetworkManager& networkManager) : m_MessageManager(messageManager), m_AudioManager(audioManager), m_ImageManager(imageManager), m_AuthManager(authManager), m_NetworkManager(networkManager), user(authManager.GetUser()), m_CurrentTime(audioManager.GetCurrentTrackPos()), m_AudioVolume(audioManager.GetVolume()), queue(audioManager.GetQueue()) {
			currentTrack = Track();
		}

		void SetupSubscriptions() override;
		void OnLoad(const std::variant<int, std::string> moduleState) override;
		void OnUpdate() override;	
		void Window() override;
		uint16_t OnRender() override;

		const int DefaultSize() const override { 
			if (queue.empty()) {
				return 0;
			} else {
				return 3;
			}
		}

		MODULE_CLASS_NAME("PlayerModule")
	private:
		void QueueView();
		void PlayerView();
		void PlaylistModal();
		void UserInfoChanged();


		uint16_t upProps = 0;

		MessageManager& m_MessageManager;
		AudioManager& m_AudioManager;
		ImageManager& m_ImageManager;
		AuthManager& m_AuthManager;
		NetworkManager& m_NetworkManager;

		bool loggedIn = false;
		const User& user;
		std::unordered_map<uint32_t, bool> selectedPlaylists;

		bool open = true;
		bool isQueueOpen = false;

		int lastClickedIndex = 0;
		Track lastClickedTrack;
		bool addTrackToPlaylist = false;
		std::list<Track>& queue;

		Track currentTrack;

		int& m_CurrentTime;
		int& m_AudioVolume;

		bool changed = false;
	};
}