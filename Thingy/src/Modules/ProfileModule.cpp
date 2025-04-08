#include "ProfileModule.h"

namespace Thingy {

	void ProfileModule::SetupSubscriptions() {
		m_MessageManager.Subscribe("loggedIn", GetModuleName(), [this](const MessageData data) {
			if (data.type() == typeid(bool)) {
				loggedIn = std::any_cast<bool>(data);
			}
			});

		m_MessageManager.Subscribe("userChanged", GetModuleName(), [this](const MessageData data) {
			UserInfoChanged();
			});
	}

	void ProfileModule::OnLoad(const std::variant<int, std::string> moduleState) {
		if (!loggedIn) {
			m_MessageManager.Publish("changeScene", std::string("FrontPage"));
		}
		UserInfoChanged();
	}

	void ProfileModule::OnUpdate() {
		if (!loggedIn) {
			m_MessageManager.Publish("changeScene", std::string("FrontPage"));
		}
	}

	void ProfileModule::Window() {
		ImGui::BeginGroup();
		CircleImage(reinterpret_cast<ImTextureID>(pfpTexture.get()), 300.0f);
		if (ImGui::IsItemHovered()) {
			upProps |= BIT(3);
		}
		if (ImGui::IsItemClicked()) {
			std::string imagePath;
			if (!OpenFileExplorer(imagePath)) {
				T_ERROR("OpenFileExplorer failed!");
			} else {
				if (IsImageFile(imagePath)) {
					std::string url = "http://localhost:3000/users/pic/" + std::to_string(user.userID);
					std::string token;
					m_AuthManager.RetrieveToken("accessToken", token);
					m_NetworkManager.UploadImage(url, imagePath, token);
					m_MessageManager.Publish("updateUser", "");
				} else {
					T_INFO("not image path: {0}", imagePath);
				}
			}
			
		}
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::Text(U8(user.username.c_str()));
		ImGui::Text(U8(user.email.c_str()));
		if(editingDesc){
			ImGui::SetKeyboardFocusHere();
			ImGui::InputText("##newDesc", &newDescription, 0, ResizeCallback, (void*)&newDescription);
			

			if (ImGui::IsItemDeactivatedAfterEdit()) {
				showDescChangePopup = true;
			}

			if (showDescChangePopup) {
				if (!ImGui::IsPopupOpen("Save Changes?")) {
					ImGui::OpenPopup("Save Changes?");
				}
				if (ImGui::BeginPopupModal("Save Changes?", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
					ImGui::Button("Yes", ImVec2(50.0f, 30.0f));
					if (ImGui::IsItemClicked()) {
						m_MessageManager.Publish("changeDescription", newDescription);
						editingDesc = false;
						showDescChangePopup = false;
						ImGui::CloseCurrentPopup();
					}
					ImGui::SameLine();
					ImGui::Button("No", ImVec2(50.0f, 30.0f));
					if (ImGui::IsItemClicked()) {
						editingDesc = false;
						showDescChangePopup = false;
						ImGui::CloseCurrentPopup();
					}
					ImGui::PopStyleColor();
					ImGui::EndPopup();
				}
			}
		} else {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
			if(ImGui::Button("Change Description", ImVec2(ImGui::CalcTextSize("Change Description").x + 5.0f, 50.0f))) {
				editingDesc = true;
				newDescription = user.description;
			};
			ImGui::PopStyleColor();
			ImGui::SameLine();
			LimitedTextWrap(user.description.c_str(), 500.0f, 3);
		}
		ImGui::EndGroup();
		ImGui::EndGroup();

		ImGui::BeginChild("Playlists", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);
		for (size_t i = 0; i < user.playlists.size(); i++) {
			ImGui::BeginGroup();
			ImGui::Image(m_ImageManager.GetImTexture(user.playlists[i].playlistID), ImVec2(200.0f, 200.0f));
			if (ImGui::IsItemClicked()) {
				m_MessageManager.Publish("openPlaylist", user.playlists[i]);
				m_MessageManager.Publish("changeScene", std::string("PlaylistScene"));
			}
			LimitedTextWrap(user.playlists[i].playlistName.c_str(), 180, 3);
			ImGui::EndGroup();
			ImGui::SameLine();
		}
		ImGui::EndChild();
	}

	uint16_t ProfileModule::OnRender() {
		upProps &= BIT(0);
		ImGui::Begin(GetModuleName().c_str(), nullptr, defaultWindowFlags);
		Window();
		ImGui::End();
		return upProps;
	}

	void ProfileModule::UserInfoChanged() {
		if (user.pfpBuffer.empty()) {
			pfpTexture = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetDefaultArtistImage());
		}
		else {
			pfpTexture = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTextureFromImage(Image(user.pfpBuffer)));
		}
		for (size_t i = 0; i < user.playlists.size(); i++) {
			const Playlist& currP = user.playlists[i];
			if (currP.playlistCoverBuffer.empty()) {
				m_ImageManager.AddTexture(currP.playlistID, m_ImageManager.GetDefaultPlaylistImage());
			} else {
				m_ImageManager.AddTexture(currP.playlistID, m_ImageManager.GetTextureFromImage(Image(currP.playlistCoverBuffer)));
			};
		}
	}
}