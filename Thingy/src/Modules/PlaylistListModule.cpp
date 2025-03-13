#include "PlaylistListModule.h"

namespace Thingy {
	void PlaylistListModule::SetupSubscriptions() {
		m_MessageManager.Subscribe("userChanged", GetModuleName(),[this](const MessageData data){
			UpdateUserInfo();
			});
		m_MessageManager.Subscribe("loggedIn", GetModuleName(), [this](const MessageData data) {
			if (data.type() == typeid(bool)) {
				loggedIn = std::any_cast<bool>(data);
			}
			});
	}
	
	void PlaylistListModule::OnLoad(const std::variant<int, std::string> moduleState) {}
	
	void PlaylistListModule::OnUpdate() {}

	void PlaylistListModule::Window() {
		ImVec2 bar_size = ImVec2(GetSize().x - 20, 30);
		ImGui::InvisibleButton("DragBar", bar_size);
		if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
			upProps |= BIT(0);
		}
		if (ImGui::IsItemHovered()) {
			upProps |= BIT(2);
		} else {
			upProps &= ~BIT(2);
		}
		if (upProps & BIT(1)) upProps &= ~BIT(1);
		if (upProps & BIT(0) && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
			upProps |= BIT(1);
			upProps &= ~BIT(0);
		}
		ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255), 0.0f, 0, 5.0f);
		
		if (ImGui::BeginTable("PlaylistTable", 2, ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY)) {
			ImGui::TableSetupColumn("Cover", ImGuiTableColumnFlags_WidthFixed, 80.0f);
			ImGui::TableSetupColumn("Info");

			int i = 0;
			for (auto& playlist : user.playlists) {

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Image(reinterpret_cast<ImTextureID>(playlistTextures[playlist.playlistID].get()), { 80.0f, 80.0f });

				ImGui::TableSetColumnIndex(1);
				std::string label = std::to_string(i);
				if (ImGui::Selectable(std::string("##" + label).data(), false, ImGuiSelectableFlags_AllowOverlap, ImVec2(0, 80.0f))) {

				}

				ImGui::SameLine();
				ImGui::BeginGroup();
				ImGui::Text(playlist.playlistName.data());
				ImGui::EndGroup();
				i++;
			}

			ImGui::EndTable();
		}

	}

	uint16_t PlaylistListModule::OnRender() {
		if (loggedIn) {
			ImGui::Begin(GetModuleName().data(), nullptr, defaultWindowFlags);
			Window();
			ImGui::End();
			if (upProps & BIT(0)) {
				ImGui::BeginDisabled();
				ImGui::Begin("floater", nullptr, defaultWindowFlags);
				Window();
				ImGui::SetWindowPos({ ImGui::GetMousePos().x - GetSize().x / 2, ImGui::GetMousePos().y + 5 });
				ImGui::SetWindowSize(GetSize());
				ImGui::End();
				ImGui::EndDisabled();
			}
		}
		return upProps;
	}

	void PlaylistListModule::UpdateUserInfo() {
		std::unordered_map<uint32_t, std::future<Image>> images;

		for (auto& playlist : user.playlists) {
			if (playlist.playlistCoverBuffer.empty()) {
				playlistTextures[playlist.playlistID] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetDefaultPlaylistImage());
			} else {
				playlistTextures[playlist.playlistID] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTextureFromImage(Image(playlist.playlistCoverBuffer)));
			}
		}
	}
}