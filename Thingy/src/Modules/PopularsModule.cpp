#include "tpch.h"
#include "PopularsModule.h"

namespace Thingy {




	void PopularsModule::OnLoad() {
		if (weeklyTracks.size() < 5) {
			GetPopulars();
			for (size_t i = 0; i < 5; i++) {
				textures[weeklyTracks[i].id] = std::unique_ptr<SDL_Texture,SDL_TDeleter>(m_ImageManager->GetTexture(weeklyTracks[i].imageURL));
				textures[monthlyTracks[i].id] = std::unique_ptr<SDL_Texture,SDL_TDeleter>(m_ImageManager->GetTexture(monthlyTracks[i].imageURL));
				textures[weeklyAlbums[i].id] = std::unique_ptr<SDL_Texture,SDL_TDeleter>(m_ImageManager->GetTexture(weeklyAlbums[i].imageURL));
				textures[monthlyAlbums[i].id] = std::unique_ptr<SDL_Texture,SDL_TDeleter>(m_ImageManager->GetTexture(monthlyAlbums[i].imageURL));
				if (weeklyArtists[i].artistImageURL == "") {
					textures[weeklyArtists[i].id] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager->GetDefaultArtistImage());
				} else {
					textures[weeklyArtists[i].id] = std::unique_ptr<SDL_Texture,SDL_TDeleter>(m_ImageManager->GetTexture(weeklyArtists[i].artistImageURL));
				}
				if (monthlyArtists[i].artistImageURL == "") {
					textures[monthlyArtists[i].id] = std::unique_ptr<SDL_Texture,SDL_TDeleter>(m_ImageManager->GetDefaultArtistImage());
				} else {
					textures[monthlyArtists[i].id] = std::unique_ptr<SDL_Texture,SDL_TDeleter>(m_ImageManager->GetTexture(monthlyArtists[i].artistImageURL));
				}
			}
		}
	}

	void PopularsModule::OnUpdate() {
	}
	void PopularsModule::Window() {

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
		ImVec2 winSize = ImGui::GetWindowSize();
		float centering = (winSize.x - 475) / 2;
		bool collapse = winSize.x > 1000 ? true : false;
		if (collapse) {

			ImGui::Text("Weekly Top Tracks");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			ImGui::Text("Monthly Top Tracks");
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[weeklyTracks[i].id].get(), {85.0f, 85.0f});
				if (ImGui::IsItemClicked()) {
					m_AudioManager->LoadMusicFromTrack(weeklyTracks[i]);
					m_AudioManager->ChangeMusic();
					m_AudioManager->ResumeMusic();
				};
				ImGui::SameLine();
			}
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[monthlyTracks[i].id].get(), { 85.0f, 85.0f });
				if (ImGui::IsItemClicked()) {
					m_AudioManager->LoadMusicFromTrack(monthlyTracks[i]);
					m_AudioManager->ChangeMusic();
					m_AudioManager->ResumeMusic();
				};
				if (i != 4)
					ImGui::SameLine();
			}
			ImGui::Text("Weekly Top Albums");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			ImGui::Text("Monthly Top Albums");
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[weeklyAlbums[i].id].get(), { 85.0f, 85.0f });
				if (ImGui::IsItemClicked()) T_INFO("Hello {0}", i);
				ImGui::SameLine();
			}
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[monthlyAlbums[i].id].get(), { 85.0f, 85.0f });
				if (ImGui::IsItemClicked()) T_INFO("Hello {0}", i);
				if (i != 4)
					ImGui::SameLine();
			}
			ImGui::Text("Weekly Top Artists");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			ImGui::Text("Monthly Top Artists");
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[weeklyArtists[i].id].get(), { 85.0f, 85.0f });
				if (ImGui::IsItemClicked()) T_INFO("Hello {0}", i);
				ImGui::SameLine();
			}
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[monthlyArtists[i].id].get(), { 85.0f, 85.0f });
				if (ImGui::IsItemClicked()) T_INFO("Hello {0}", i);
				if (i != 4)
					ImGui::SameLine();
			}
		} else {
			
			ImGui::SetCursorPosX(centering);
			ImGui::Text("Weekly Top Tracks");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[weeklyTracks[i].id].get(), { 85.0f, 85.0f });
				if (ImGui::IsItemClicked()) {
					m_AudioManager->LoadMusicFromTrack(weeklyTracks[i]);
					m_AudioManager->ChangeMusic();
					m_AudioManager->ResumeMusic();
				};
				if (i != 4)
				ImGui::SameLine();
			}

			ImGui::SetCursorPosX(centering);
			ImGui::Text("Monthly Top Tracks");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[monthlyTracks[i].id].get(), { 85.0f, 85.0f });
				if (ImGui::IsItemClicked()) {
					m_AudioManager->LoadMusicFromTrack(monthlyTracks[i]);
					m_AudioManager->ChangeMusic();
					m_AudioManager->ResumeMusic();
				};
				if (i != 4)
					ImGui::SameLine();
			}

			ImGui::SetCursorPosX(centering);
			ImGui::Text("Weekly Top Albums");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[weeklyAlbums[i].id].get(), { 85.0f, 85.0f });
				if (ImGui::IsItemClicked()) T_INFO("Hello {0}", i);
				if (i != 4)
				ImGui::SameLine();
			}
			
			ImGui::SetCursorPosX(centering);
			ImGui::Text("Monthly Top Albums");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[monthlyAlbums[i].id].get(), { 85.0f, 85.0f });
				if (ImGui::IsItemClicked()) T_INFO("Hello {0}", i);
				if (i != 4)
					ImGui::SameLine();
			}

			ImGui::SetCursorPosX(centering);
			ImGui::Text("Weekly Top Artists");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[weeklyArtists[i].id].get(), { 85.0f, 85.0f });
				if (ImGui::IsItemClicked()) T_INFO("Hello {0}", i);
				if (i != 4)
				ImGui::SameLine();
			}

			ImGui::SetCursorPosX(centering);
			ImGui::Text("Monthly Top Artists");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[monthlyArtists[i].id].get(), { 85.0f, 85.0f });
				if (ImGui::IsItemClicked()) T_INFO("Hello {0}", i);
				if (i != 4)
					ImGui::SameLine();
			}
		}
		ImGui::Text("window size: (%.1f, %.1f)", ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
	}



	uint16_t PopularsModule::OnRender() {
		ImGui::Begin(GetModuleName().data(), nullptr, defaultWindowFlags);
		ImVec2 size = GetSize();
		size.x = ImClamp(size.x, (float)MinWidth(), (float)MaxWidth());
		ImGui::SetWindowSize(size);
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
		return upProps;
	}

	void PopularsModule::GetPopulars() {
		std::string weeklyTrack = "https://api.jamendo.com/v3.0/tracks/?client_id=8b1de417&format=jsonpretty&order=popularity_week&limit=5";
		std::string weeklyAlbum = "https://api.jamendo.com/v3.0/albums/?client_id=8b1de417&format=jsonpretty&order=popularity_week&limit=5";
		std::string weeklyArtist = "https://api.jamendo.com/v3.0/artists/?client_id=8b1de417&format=jsonpretty&order=popularity_week&limit=5";
		std::string monthlyTrack = "https://api.jamendo.com/v3.0/tracks/?client_id=8b1de417&format=jsonpretty&order=popularity_month&limit=5";
		std::string monthlyAlbum = "https://api.jamendo.com/v3.0/albums/?client_id=8b1de417&format=jsonpretty&order=popularity_month&limit=5";
		std::string monthlyArtist = "https://api.jamendo.com/v3.0/artists/?client_id=8b1de417&format=jsonpretty&order=popularity_month&limit=5";

		std::future<std::vector<Track>> futureWeeklyTracks = std::async(std::launch::async, &NetworkManager::GetTrack, m_NetworkManager.get(), weeklyTrack);
		std::future<std::vector<Album>> futureWeeklyAlbums = std::async(std::launch::async, &NetworkManager::GetAlbum, m_NetworkManager.get(), weeklyAlbum);
		std::future<std::vector<Artist>> futureWeeklyArtists = std::async(std::launch::async, &NetworkManager::GetArtist, m_NetworkManager.get(), weeklyArtist);
		std::future<std::vector<Track>> futureMonthlyTracks = std::async(std::launch::async, &NetworkManager::GetTrack, m_NetworkManager.get(), monthlyTrack);
		std::future<std::vector<Album>> futureMonthlyAlbums = std::async(std::launch::async, &NetworkManager::GetAlbum, m_NetworkManager.get(), monthlyAlbum);
		std::future<std::vector<Artist>> futureMonthlyArtists = std::async(std::launch::async, &NetworkManager::GetArtist, m_NetworkManager.get(), monthlyArtist);
		
		weeklyTracks = futureWeeklyTracks.get();
		weeklyAlbums = futureWeeklyAlbums.get();
		weeklyArtists = futureWeeklyArtists.get();
		monthlyTracks = futureMonthlyTracks.get();
		monthlyAlbums = futureMonthlyAlbums.get();
		monthlyArtists = futureMonthlyArtists.get();
	}
	
}