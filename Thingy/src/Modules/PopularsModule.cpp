#include "tpch.h"
#include "PopularsModule.h"

namespace Thingy {




	void PopularsModule::OnUpdate() {

		GetPopulars();
	}
	void PopularsModule::Window(std::string title) {
		ImGui::Begin(title.data(), nullptr, defaultWindowFlags);
		ImVec2 bar_size = ImVec2(GetSize().x - 20, 30);
		ImGui::InvisibleButton("DragBar", bar_size);
		ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255), 0.0f, 0, 5.0f);
		ImVec2 winSize = ImGui::GetWindowSize();
		float centering = (winSize.x - 475) / 2;
		bool collapse = winSize.x > 1000 ? true : false;
		if (collapse) {

			ImGui::Text("Weekly Top Tracks");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			ImGui::Text("Monthly Top Tracks");
			for (size_t i = 0; i < 5; i++) {
				//ImGui::Image((ImTextureID)(intptr_t)GetTexture(weeklyTracks[i].imageURL), {85.0f, 85.0f});
				ImGui::Button("hi1", { 85.0f, 85.0f });
				ImGui::SameLine();
			}
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			for (size_t i = 0; i < 5; i++) {
				//ImGui::Image((ImTextureID)(intptr_t)GetTexture(monthlyTracks[i].imageURL), { 85.0f, 85.0f });
				ImGui::Button("hi2", { 85.0f, 85.0f });
				if (i != 4)
					ImGui::SameLine();
			}
			ImGui::Text("Weekly Top Albums");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			ImGui::Text("Monthly Top Albums");
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi3", { 85.0f, 85.0f });
				ImGui::SameLine();
			}
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi4", { 85.0f, 85.0f });
				if (i != 4)
					ImGui::SameLine();
			}
			ImGui::Text("Weekly Top Artists");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			ImGui::Text("Monthly Top Artists");
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi5", { 85.0f, 85.0f });
				ImGui::SameLine();
			}
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi6", { 85.0f, 85.0f });
				if (i != 4)
					ImGui::SameLine();
			}
		} else {
			
			ImGui::SetCursorPosX(centering);
			ImGui::Text("Weekly Top Tracks");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi", { 85.0f, 85.0f });
				if (i != 4)
				ImGui::SameLine();
			}

			ImGui::SetCursorPosX(centering);
			ImGui::Text("Monthly Top Tracks");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi2", { 85.0f, 85.0f });
				if (i != 4)
					ImGui::SameLine();
			}

			ImGui::SetCursorPosX(centering);
			ImGui::Text("Weekly Top Albums");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi3", { 85.0f, 85.0f });
				if (i != 4)
				ImGui::SameLine();
			}
			
			ImGui::SetCursorPosX(centering);
			ImGui::Text("Monthly Top Albums");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi4", { 85.0f, 85.0f });
				if (i != 4)
					ImGui::SameLine();
			}

			ImGui::SetCursorPosX(centering);
			ImGui::Text("Weekly Top Artists");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi5", { 85.0f, 85.0f });
				if (i != 4)
				ImGui::SameLine();
			}

			ImGui::SetCursorPosX(centering);
			ImGui::Text("Monthly Top Artists");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi6", { 85.0f, 85.0f });
				if (i != 4)
					ImGui::SameLine();
			}
		}
		ImGui::Text("window size: (%.1f, %.1f)", ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
		ImGui::End();
	}
	void PopularsModule::OnRender() {
		Window(GetModuleName().data());
	}
	void PopularsModule::GetPopulars() {
		std::string weeklyTrack = "https://api.jamendo.com/v3.0/tracks/?client_id=8b1de417&format=jsonpretty&order=popularity_week&limit=5";
		std::string weeklyAlbum = "https://api.jamendo.com/v3.0/albums/?client_id=8b1de417&format=jsonpretty&order=popularity_week&limit=5";
		std::string weeklyArtist = "https://api.jamendo.com/v3.0/artists/?client_id=8b1de417&format=jsonpretty&order=popularity_week&limit=5";
		std::string monthlyTrack = "https://api.jamendo.com/v3.0/tracks/?client_id=8b1de417&format=jsonpretty&order=popularity_month&limit=5";
		std::string monthlyAlbum = "https://api.jamendo.com/v3.0/albums/?client_id=8b1de417&format=jsonpretty&order=popularity_month&limit=5";
		std::string monthlyArtist = "https://api.jamendo.com/v3.0/artists/?client_id=8b1de417&format=jsonpretty&order=popularity_month&limit=5";
		weeklyTracks = m_NetworkManager->GetTrack(weeklyTrack);
		weeklyAlbums = m_NetworkManager->GetAlbum(weeklyAlbum);
		weeklyArtists = m_NetworkManager->GetArtist(weeklyArtist);
		monthlyTracks = m_NetworkManager->GetTrack(monthlyTrack);
		monthlyAlbums = m_NetworkManager->GetAlbum(monthlyAlbum);
		monthlyArtists = m_NetworkManager->GetArtist(monthlyArtist);
	}
	SDL_Texture* PopularsModule::GetTexture(std::string& url) {
		std::vector<unsigned char> buffer;
		m_NetworkManager->DownloadImage(url, buffer);
		Image image(buffer);
		SDL_Texture* texture = image.createTexture(m_Renderer);
		return texture;
	}
}