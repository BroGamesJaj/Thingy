#include "ProfileModule.h"

namespace Thingy {

	void ProfileModule::SetupSubscriptions() {
		m_MessageManager->Subscribe("loggedIn", GetModuleName(), [this](const MessageData data) {
			if (data.type() == typeid(bool)) {
				loggedIn = std::any_cast<bool>(data);
			}
			});
	}

	void ProfileModule::OnLoad(const std::variant<int, std::string> moduleState) {
		if (!loggedIn) {
			pfp.reset();
			user = User();
			m_MessageManager->Publish("changeScene", std::string("FrontPage"));
		}
		user = m_AuthManager->GetUser();
		if (user.pfpBuffer.empty()) {
			pfp = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager->GetDefaultArtistImage());
		} else {
			pfp = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager->GetTextureFromImage(Image(user.pfpBuffer)));
		}
		for (size_t i = 0; i < user.playlists.size(); i++) {
			Playlist& currP = user.playlists[i];
			if (currP.playlistCoverBuffer.empty()) {
				playlistCovers[currP.playlistID] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager->GetDefaultPlaylistImage());
			} else {
				
				playlistCovers[currP.playlistID] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager->GetTextureFromImage(Image(currP.playlistCoverBuffer)));
			};
		}
	}

	void ProfileModule::OnUpdate() {
		if (!loggedIn) {
			pfp.reset();
			m_MessageManager->Publish("changeScene", std::string("FrontPage"));
		}
	}

	void ProfileModule::Window() {
		ImGui::BeginGroup();
		ImGui::Image((ImTextureID)(intptr_t)pfp.get(), ImVec2(300.0f, 300.0f));
		ImGui::SameLine();
		LimitedTextWrap(user.description.data(), 500.0f, 3);
		ImGui::EndGroup();

		ImGui::BeginChild("Playlists", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);
		for (size_t i = 0; i < user.playlists.size(); i++) {
			ImGui::BeginGroup();
			ImGui::Image((ImTextureID)(intptr_t)playlistCovers[user.playlists[i].playlistID].get(), ImVec2(200.0f, 200.0f));
			LimitedTextWrap(user.playlists[i].playlistName.data(), 180, 3);
			ImGui::EndGroup();
			ImGui::SameLine();
		}
		ImGui::EndChild();
	}

	uint16_t ProfileModule::OnRender() {
		ImGui::Begin(GetModuleName().data(), nullptr, defaultWindowFlags);
		Window();
		ImGui::End();
		return upProps;
	}

}