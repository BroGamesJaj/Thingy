#include "tpch.h"
#include "PlayerModule.h"
namespace Thingy {
	void PlayerModule::OnUpdate() {

	}

	void PlayerModule::Window(std::string title) {
		ImGui::Begin(title.data(), nullptr, defaultWindowFlags);
		ImGui::Button("image", { 300.0f, 300.0f });
		ImGui::Text("Thing name");
		if (ImGui::Button("back", { 30.0f, 30.0f })) {
			m_AudioManager->PrevTrack();
		}
		ImGui::SameLine();
		if (m_AudioManager->IsMusicPaused()) {
			if (ImGui::Button("Play", { 30.0f, 30.0f })) {
				m_AudioManager->ResumeMusic();
			}
		}
		else {
			if (ImGui::Button("Pause", { 30.0f, 30.0f })) {
				m_AudioManager->PauseMusic();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Forward", { 30.0f, 30.0f })) {
			m_AudioManager->NextTrack();
		};
		ImGui::Text("Current");
		ImGui::SameLine();
		ImGui::SliderInt("time", &m_CurrentTime, 0, m_AudioManager->GetCurrentTrackDuration());
		if (ImGui::IsItemEdited()) {
			m_AudioManager->ChangeMusicPos();
		}
		ImGui::SliderInt("Volume", &m_AudioVolume, 0, MIX_MAX_VOLUME);
		if (ImGui::IsItemEdited()) {
			m_AudioManager->ChangeVolume();
		}

		ImGui::End();
	}

	void PlayerModule::OnRender() {
		Window(GetModuleName().data());
	}
}