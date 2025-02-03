#include "tpch.h"
#include "PopularsModule.h"

namespace Thingy {




	void PopularsModule::OnLoad() {
		if (weeklyTracks.size() < 5) {
			GetPopulars();
			for (size_t i = 0; i < 5; i++) {
				std::future<SDL_Texture*> wt = std::async(std::launch::async, [=]() { return GetTexture(weeklyTracks[i].imageURL); });
				std::future<SDL_Texture*> mt = std::async(std::launch::async, [=]() { return GetTexture(monthlyTracks[i].imageURL); });
				std::future<SDL_Texture*> wal = std::async(std::launch::async, [=]() { return GetTexture(weeklyAlbums[i].imageURL); });
				std::future<SDL_Texture*> mal = std::async(std::launch::async, [=]() { return GetTexture(monthlyAlbums[i].imageURL); });
				//textures[weeklyTracks[i].id] = std::unique_ptr<SDL_Texture,SDL_TDeleter>(GetTexture(weeklyTracks[i].imageURL));
				//textures[monthlyTracks[i].id] = std::unique_ptr<SDL_Texture,SDL_TDeleter>(GetTexture(monthlyTracks[i].imageURL));
				//textures[weeklyAlbums[i].id] = std::unique_ptr<SDL_Texture,SDL_TDeleter>(GetTexture(weeklyAlbums[i].imageURL));
				//textures[monthlyAlbums[i].id] = std::unique_ptr<SDL_Texture,SDL_TDeleter>(GetTexture(monthlyAlbums[i].imageURL));
				if (weeklyArtists[i].artistImageURL == "") {
					textures[weeklyArtists[i].id] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(GetDefaultArtistImage());
				} else {
					textures[weeklyArtists[i].id] = std::unique_ptr<SDL_Texture,SDL_TDeleter>(GetTexture(weeklyArtists[i].artistImageURL));
				}
				if (monthlyArtists[i].artistImageURL == "") {
					textures[monthlyArtists[i].id] = std::unique_ptr<SDL_Texture,SDL_TDeleter>(GetDefaultArtistImage());
				} else {
					textures[monthlyArtists[i].id] = std::unique_ptr<SDL_Texture,SDL_TDeleter>(GetTexture(monthlyArtists[i].artistImageURL));
				}
				textures[weeklyTracks[i].id] = std::unique_ptr<SDL_Texture,SDL_TDeleter>(wt.get());
				textures[monthlyTracks[i].id] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(mt.get());
				textures[weeklyAlbums[i].id] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(wal.get());
				textures[monthlyAlbums[i].id] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(mal.get());
			}
		}
	}

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
				ImGui::Image((ImTextureID)(intptr_t)textures[weeklyTracks[i].id].get(), {85.0f, 85.0f});
				ImGui::SameLine();
			}
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[monthlyTracks[i].id].get(), { 85.0f, 85.0f });
				if (i != 4)
					ImGui::SameLine();
			}
			ImGui::Text("Weekly Top Albums");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			ImGui::Text("Monthly Top Albums");
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[weeklyAlbums[i].id].get(), { 85.0f, 85.0f });
				ImGui::SameLine();
			}
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[monthlyAlbums[i].id].get(), { 85.0f, 85.0f });
				if (i != 4)
					ImGui::SameLine();
			}
			ImGui::Text("Weekly Top Artists");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			ImGui::Text("Monthly Top Artists");
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[weeklyArtists[i].id].get(), { 85.0f, 85.0f });
				ImGui::SameLine();
			}
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[monthlyArtists[i].id].get(), { 85.0f, 85.0f });
				if (i != 4)
					ImGui::SameLine();
			}
		} else {
			
			ImGui::SetCursorPosX(centering);
			ImGui::Text("Weekly Top Tracks");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[weeklyTracks[i].id].get(), { 85.0f, 85.0f });
				if (i != 4)
				ImGui::SameLine();
			}

			ImGui::SetCursorPosX(centering);
			ImGui::Text("Monthly Top Tracks");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[monthlyTracks[i].id].get(), { 85.0f, 85.0f });
				if (i != 4)
					ImGui::SameLine();
			}

			ImGui::SetCursorPosX(centering);
			ImGui::Text("Weekly Top Albums");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[weeklyAlbums[i].id].get(), { 85.0f, 85.0f });
				if (i != 4)
				ImGui::SameLine();
			}
			
			ImGui::SetCursorPosX(centering);
			ImGui::Text("Monthly Top Albums");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[monthlyAlbums[i].id].get(), { 85.0f, 85.0f });
				if (i != 4)
					ImGui::SameLine();
			}

			ImGui::SetCursorPosX(centering);
			ImGui::Text("Weekly Top Artists");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[weeklyArtists[i].id].get(), { 85.0f, 85.0f });
				if (i != 4)
				ImGui::SameLine();
			}

			ImGui::SetCursorPosX(centering);
			ImGui::Text("Monthly Top Artists");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Image((ImTextureID)(intptr_t)textures[monthlyArtists[i].id].get(), { 85.0f, 85.0f });
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
	SDL_Texture* PopularsModule::GetTexture(std::string& url) {
		std::vector<unsigned char> buffer;
		m_NetworkManager->DownloadImage(url, buffer);
		Image image(buffer);
		SDL_Texture* texture = image.createTexture(m_Renderer);
		return texture;
	}

	SDL_Texture* PopularsModule::GetDefaultArtistImage() {
		
		SDL_Texture* texture;
		int w, h;
		LoadTextureFromFile("../assets/images/defaultArtist.png", m_Renderer, &texture, &w, &h);
		return texture;
	}
}