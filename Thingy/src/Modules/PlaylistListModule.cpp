#include "PlaylistListModule.h"

namespace Thingy {
	void PlaylistListModule::SetupSubscriptions() {
		m_MessageManager.Subscribe("userChanged", GetModuleName(), [this](const MessageData data) {
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
		ImVec2 barSize = ImVec2(GetSize().x - 20, 30);
		DragBar(upProps, barSize);

		if (ImGui::BeginTable("PlaylistTable", 2, ImGuiTableFlags_ScrollY, ImVec2(0, ImGui::GetWindowSize().y - 100.0f))) {
			ImGui::TableSetupColumn("Cover", ImGuiTableColumnFlags_WidthFixed, 80.0f);
			ImGui::TableSetupColumn("Info");

			int i = 0;
			for (auto& playlist : user.playlists) {

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Image(reinterpret_cast<ImTextureID>(playlistTextures[playlist.playlistID].get()), { 80.0f, 80.0f });

				ImGui::TableSetColumnIndex(1);
				std::string label = std::to_string(i);
				if (ImGui::Selectable(std::string("##" + label).c_str(), false, ImGuiSelectableFlags_AllowOverlap, ImVec2(0, 80.0f))) {
					m_MessageManager.Publish("openPlaylist", playlist);
					m_MessageManager.Publish("changeScene", std::string("PlaylistScene"));
				}

				ImGui::SameLine();
				ImGui::BeginGroup();
				ImGui::Text(U8(playlist.playlistName.c_str()));
				ImGui::EndGroup();
				i++;
			}

			ImGui::EndTable();
		}
		if (ImGui::Button("Create New Playlist")) {
			error = "";
			newDescription = "";
			newPlaylistName = "";
			isPrivate = false;
			coverPath = "";
			newPlaylistCover = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetDefaultPlaylistImage());
			ImGui::OpenPopup("New playlist");
		}

		NewPlaylistModal();

	}

	uint16_t PlaylistListModule::OnRender() {
		upProps &= BIT(0);
		if (loggedIn) {
			ImGui::Begin(GetModuleName().c_str(), nullptr, defaultWindowFlags);
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

	void PlaylistListModule::NewPlaylistModal() {
		ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImVec2 modalSize = ImVec2(310.0f, 600.0f);
		ImVec2 modalPos = ImVec2(center.x - modalSize.x * 0.5f, center.y - modalSize.y * 0.5f);
		ImGui::SetNextWindowPos(modalPos);
		ImGui::SetNextWindowSize(modalSize);

		if (ImGui::BeginPopupModal("New playlist", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
			ImGui::SameLine(ImGui::GetWindowWidth() - 20);
			if (ImGui::Button("X")) ImGui::CloseCurrentPopup();

			ImGui::Image((ImTextureID)(intptr_t)newPlaylistCover.get(), ImVec2(100.0f, 100.0f));
			if (ImGui::IsItemHovered()) {
				upProps |= BIT(3);
			}
			if (ImGui::IsItemClicked()) {
				std::string imagePath;
				if (!OpenFileExplorer(imagePath)) {
					T_ERROR("OpenFileExplorer failed!");
				} else {
					if (IsImageFile(imagePath)) {
						coverPath = imagePath;
						newPlaylistCover = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTextureFromFile(coverPath.c_str()));
						
					} else {
						T_INFO("not image path: {0}", imagePath);
					}
				}

			}
			ImGui::InputText("Playlist Name", &newPlaylistName, 0, ResizeCallback, (void*)&newPlaylistName);
			ImGui::InputText("Description", &newDescription, 0, ResizeCallback, (void*)&newDescription);
			ImGui::Checkbox("Private?", &isPrivate);
			if (ImGui::Button("Save Playlist")) {
				if (newPlaylistName.empty()) {
					error = "Playlist name cannot be empty!";
				} else {
					std::string url = "http://localhost:3000/playlists";
					std::string token;
					m_AuthManager.RetrieveToken("accessToken", token);
					m_NetworkManager.AddPlaylist(url, newPlaylistName, newDescription, isPrivate, coverPath, token);
					m_MessageManager.Publish("updateUser", "");
					ImGui::CloseCurrentPopup();
				}
			}
			if (!error.empty()) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImGui::Text(error.c_str());
				ImGui::PopStyleColor();
			}
			ImGui::EndPopup();
		}
	}
}