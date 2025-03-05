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
		PlayerModule(std::unique_ptr<MessageManager>& messageManager, std::unique_ptr<AudioManager>& audioManager, std::unique_ptr<ImageManager>& imageManager) : m_MessageManager(messageManager), m_AudioManager(audioManager), m_ImageManager(imageManager), m_CurrentTime(audioManager->GetCurrentTrackPos()), m_AudioVolume(audioManager->GetVolume()) {
			m_TrackID = 0;
			m_TrackName = "";
			m_TrackArtist = "";
			m_TrackDuration = 0;
		}

		void SetupSubscriptions() override;
		void OnLoad(const std::variant<int, std::string> moduleState) override;
		void OnUpdate() override;	
		void Window() override;
		uint16_t OnRender() override;

		int DefaultWidth() const override { return 400; }

		void SetCurrentTrack(Track currTrack) {
			m_TrackID = currTrack.id;
			m_TrackName = currTrack.title;
			m_TrackArtist = currTrack.artistName;
			m_TrackDuration = currTrack.duration;
		}



		MODULE_CLASS_NAME("PlayerModule")
	private:

		struct SDL_TDeleter { void operator()(SDL_Texture* p) { SDL_DestroyTexture(p); } };

		uint16_t upProps = 0;

		std::unique_ptr<MessageManager>& m_MessageManager;
		std::unique_ptr<AudioManager>& m_AudioManager;
		std::unique_ptr<ImageManager>& m_ImageManager;

		int m_TrackID;
		std::string m_TrackName;
		std::string m_TrackArtist;
		std::unique_ptr<SDL_Texture, SDL_TDeleter> image;
		int m_TrackDuration;

		int& m_CurrentTime;
		int& m_AudioVolume;

		bool changed = false;
	};
}