#include "tpch.h"
#include "PlayerModule.h"
namespace Thingy {

	void PlayerModule::SetupSubscriptions() {}

	void PlayerModule::OnLoad() {
			
	}

	void PlayerModule::OnUpdate() {
		int currentTrackID = m_AudioManager->GetCurrentTrack().id; 

		if (currentTrackID != m_TrackID) {
			
			changed = true;
			m_TrackID = currentTrackID;

			image = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager->GetTexture(m_AudioManager->GetCurrentTrack().imageURL));

		} else {
			changed = false;
		}
	}

	void PlayerModule::Window() {


		ImVec2 bar_size = ImVec2(GetSize().x - 20, 30);
		ImGui::InvisibleButton("DragBar", bar_size);
		if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
			upProps |= BIT(0);
		}
		if (upProps & BIT(1)) upProps &= ~BIT(1);
		if (upProps & BIT(0) && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
			upProps |= BIT(1);
			upProps &= ~BIT(0);
		}
		ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255), 0.0f, 0, 5.0f);
		if (m_AudioManager->GetQueue().size() == 0) {
			ImGui::Button("image", { 300.0f, 300.0f });
		} else {
			ImGui::Image((ImTextureID)(intptr_t)image.get(), {300.0f, 300.0f});
		}
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
		ImGui::SliderInt("time", &m_AudioManager->GetCurrentTrackPos(), 0, m_AudioManager->GetCurrentTrackDuration());
		if (ImGui::IsItemEdited()) {
			m_AudioManager->ChangeMusicPos();
		}
		ImGui::SliderInt("Volume", &m_AudioVolume, 0, MIX_MAX_VOLUME);
		if (ImGui::IsItemEdited()) {
			m_AudioManager->ChangeVolume();
		}
		
	}

	uint16_t PlayerModule::OnRender() {
		ImGui::Begin(GetModuleName().data(), nullptr, defaultWindowFlags);
		Window();
		ImGui::End();
		if (upProps & BIT(0)) {
			ImGui::BeginDisabled();
			ImGui::Begin("floater", nullptr, defaultWindowFlags);
			Window();
			ImGui::SetWindowPos({ ImGui::GetMousePos().x - (ImGui::FindWindowByName(GetModuleName().data())->Size.x / 2), ImGui::GetMousePos().y + 5 });
			ImGui::SetWindowSize(ImGui::FindWindowByName(GetModuleName().data())->Size);
			ImGui::End();
			ImGui::EndDisabled();
		}
		return upProps;
	}
}