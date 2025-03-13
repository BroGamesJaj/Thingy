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
			pfp.reset();
			m_MessageManager.Publish("changeScene", std::string("FrontPage"));
		}
		UserInfoChanged();
	}

	void ProfileModule::OnUpdate() {
		if (!loggedIn) {
			pfp.reset();
			m_MessageManager.Publish("changeScene", std::string("FrontPage"));
		}
	}

	void ProfileModule::Window() {
		ImGui::BeginGroup();
		CircleImage((ImTextureID)(intptr_t)pfp.get(), 300.0f);
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
				if (ImGui::BeginPopupModal("Save Changes?", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
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
					ImGui::EndPopup();
				}
			}
		} else {

			if(ImGui::Button("descChange", ImVec2(50.0f, 50.0f))){
				editingDesc = true;
				newDescription = user.description;
			};
			ImGui::SameLine();
			LimitedTextWrap(user.description.data(), 500.0f, 3);
		}
		ImGui::EndGroup();

		ImGui::BeginChild("Playlists", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);
		for (size_t i = 0; i < user.playlists.size(); i++) {
			ImGui::BeginGroup();
			ImGui::Image(reinterpret_cast<ImTextureID>(playlistCovers[user.playlists[i].playlistID].get()), ImVec2(200.0f, 200.0f));
			LimitedTextWrap(user.playlists[i].playlistName.data(), 180, 3);
			ImGui::EndGroup();
			ImGui::SameLine();
		}
		ImGui::EndChild();
	}

	uint16_t ProfileModule::OnRender() {
		upProps = 0;
		ImGui::Begin(GetModuleName().data(), nullptr, defaultWindowFlags);
		Window();
		ImGui::End();
		return upProps;
	}

	void ProfileModule::UserInfoChanged() {
		if (user.pfpBuffer.empty()) {
			pfp = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetDefaultArtistImage());
		} else {
			pfp = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTextureFromImage(Image(user.pfpBuffer)));
		}
		for (size_t i = 0; i < user.playlists.size(); i++) {
			const Playlist& currP = user.playlists[i];
			if (currP.playlistCoverBuffer.empty()) {
				playlistCovers[currP.playlistID] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetDefaultPlaylistImage());
			} else {

				playlistCovers[currP.playlistID] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTextureFromImage(Image(currP.playlistCoverBuffer)));
			};
		}
	}
}