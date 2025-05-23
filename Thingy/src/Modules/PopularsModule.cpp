#include "tpch.h"
#include "PopularsModule.h"

namespace Thingy {

	void PopularsModule::SetupSubscriptions() {
		m_MessageManager.Subscribe("loggedIn", GetModuleName(), [this](const MessageData data) {
			if (data.type() == typeid(bool)) {
				loggedIn = std::any_cast<bool>(data);
			}
			});
	}

	void PopularsModule::OnLoad(const std::variant<int, std::string> moduleState) {
		
		if (weeklyTracks.size() == 0 || lastFetch == 0 || lastFetch < std::chrono::system_clock::to_time_t((std::chrono::system_clock::now() - std::chrono::hours(1)))) {
			GetPopulars();
			for (size_t i = 0; i < 5; i++) {
				if (i < weeklyTracks.size())
					m_ImageManager.AddTexture(weeklyTracks[i].id, m_ImageManager.GetTexture(weeklyTracks[i].imageURL));
				if(i < monthlyTracks.size())
					m_ImageManager.AddTexture(monthlyTracks[i].id, m_ImageManager.GetTexture(monthlyTracks[i].imageURL));
				if(i < weeklyAlbums.size())
					m_ImageManager.AddTexture(weeklyAlbums[i].id, m_ImageManager.GetTexture(weeklyAlbums[i].imageURL));
				if(i < monthlyAlbums.size())
					m_ImageManager.AddTexture(monthlyAlbums[i].id, m_ImageManager.GetTexture(monthlyAlbums[i].imageURL));
				
				if (i < weeklyArtists.size()) {
					if (weeklyArtists[i].artistImageURL == "") {
						m_ImageManager.AddTexture(weeklyArtists[i].id, m_ImageManager.GetDefaultArtistImage());
					} else {
						m_ImageManager.AddTexture(weeklyArtists[i].id, m_ImageManager.GetTexture(weeklyArtists[i].artistImageURL));
					}
				}
				if (i < monthlyArtists.size()) {
					if (monthlyArtists[i].artistImageURL == "") {
						m_ImageManager.AddTexture(monthlyArtists[i].id, m_ImageManager.GetDefaultArtistImage());
					} else {
						m_ImageManager.AddTexture(monthlyArtists[i].id, m_ImageManager.GetTexture(monthlyArtists[i].artistImageURL));
					}
				}
			}
			lastFetch = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		}
		
	}

	void PopularsModule::OnUpdate() {
	}
	void PopularsModule::Window() {
		ImVec4 normalColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, normalColor);

		float scale = (ImGui::GetWindowSize().x / 1280);
		float width = std::clamp(100.0f * scale, 85.0f, 125.0f);
		float height = std::clamp(100.0f * scale, 85.0f, 125.0f);
		ImVec2 imageSize = { width , height};

		ImVec2 barSize = ImVec2(GetSize().x - 20, 30);
		DragBar(upProps, barSize);ImVec2 winSize = ImGui::GetWindowSize();

		float centering = (winSize.x / 2) - (winSize.x / 2 / (5.0f / 4.2f));
		bool collapse = winSize.x < 1000 ? true : false;
		if (collapse) {

			scale = (winSize.x / 1000);
			width = std::clamp(130.0f * scale, 85.0f, 130.0f);
			height = std::clamp(130.0f * scale, 85.0f, 130.0f);
			imageSize = { width , height };
		}
		if (!collapse) {
			ImGui::SetCursorPosX(20);
			if (ImGui::BeginTable("Weeklys", 5, ImGuiTableFlags_ScrollY, {winSize.x / 2 - 20, 0.0f})) {
				ImGui::TableSetupColumn("WeeklyTracks", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableNextColumn();
				ImGui::Text("Weekly");
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Tracks");
				ImGui::TableNextRow();
				for (size_t i = 0; i < 5; i++) {
					ImGui::TableNextColumn();
					if(ImGui::ImageButton(std::string("##weeklyT" + i).c_str(), m_ImageManager.GetImTexture(weeklyTracks[i].id), imageSize)) {
						m_MessageManager.Publish("startMusic", weeklyTracks[i]);
					};
					LimitedTextWrap(weeklyTracks[i].title.c_str(), width, 3);
				}

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Albums");
				ImGui::TableNextRow();
				for (size_t i = 0; i < 5; i++) {
					ImGui::TableNextColumn();
					if (ImGui::ImageButton(std::string("##weeklyA" + i).c_str(), m_ImageManager.GetImTexture(weeklyAlbums[i].id), imageSize)) {
						m_MessageManager.Publish("openAlbum", weeklyAlbums[i]);
						m_MessageManager.Publish("changeScene", std::string("AlbumScene"));
					}
					LimitedTextWrap(weeklyAlbums[i].name.c_str(), width, 3);
				}

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Artists");
				ImGui::TableNextRow();
				for (size_t i = 0; i < 5; i++) {
					ImGui::TableNextColumn();
					if (ImGui::ImageButton(std::string("##weeklyAr" + i).c_str(), m_ImageManager.GetImTexture(weeklyArtists[i].id), imageSize)) {
						m_MessageManager.Publish("openArtist", weeklyArtists[i]);
						m_MessageManager.Publish("changeScene", std::string("ArtistScene"));
					}
					LimitedTextWrap(weeklyArtists[i].artistName.c_str(), width, 3);
				}
				ImGui::EndTable();
			}
			ImGui::SameLine();
			ImGui::SetCursorPosX(winSize.x / 2 + 15);
			if (ImGui::BeginTable("Monthlys", 5, ImGuiTableFlags_ScrollY, { winSize.x / 2 - 20, 0.0f })) {
				ImGui::TableNextColumn();
				ImGui::Text("Monthly");
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Tracks");
				ImGui::TableNextRow();
				for (size_t i = 0; i < 5; i++) {
					ImGui::TableNextColumn();
					if (ImGui::ImageButton(std::string("##monthlyT" + i).c_str(), m_ImageManager.GetImTexture(monthlyTracks[i].id), imageSize)) {
						m_MessageManager.Publish("startMusic", monthlyTracks[i]);
					};
					LimitedTextWrap(monthlyTracks[i].title.c_str(), width, 3);
				}
				
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Albums");
				ImGui::TableNextRow();
				for (size_t i = 0; i < 5; i++) {
					ImGui::TableNextColumn();
					if (ImGui::ImageButton(std::string("##monthlyA" + i).c_str(), m_ImageManager.GetImTexture(monthlyAlbums[i].id), imageSize)) {
						m_MessageManager.Publish("openAlbum", monthlyAlbums[i]);
						m_MessageManager.Publish("changeScene", std::string("AlbumScene"));
					}
					LimitedTextWrap(monthlyAlbums[i].name.c_str(), width, 3);
				}

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Artists");
				ImGui::TableNextRow();
				for (size_t i = 0; i < 5; i++) {
					ImGui::TableNextColumn();
					if (ImGui::ImageButton(std::string("##monthlyAr" + i).c_str(), m_ImageManager.GetImTexture(monthlyArtists[i].id), imageSize)) {
						m_MessageManager.Publish("openArtist", monthlyArtists[i]);
						m_MessageManager.Publish("changeScene", std::string("ArtistScene"));
					}
					LimitedTextWrap(monthlyArtists[i].artistName.c_str(), width, 3);
				}
				ImGui::EndTable();
			}

		} else {
			ImGui::SetCursorPosX(centering);
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10, 10));
			
			if (ImGui::BeginTable("WeeklysAndMonthlys", 5, ImGuiTableFlags_ScrollY, { winSize.x / (5.0f/4.2f), 0.0f})) {
				ImGui::TableNextColumn();
				ImGui::Text("Weekly");
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Tracks");
				ImGui::TableNextRow();
				for (size_t i = 0; i < 5; i++) {
					ImGui::TableNextColumn();
					if (ImGui::ImageButton(std::string("##weeklyT" + i).c_str(), m_ImageManager.GetImTexture(weeklyTracks[i].id), imageSize)) {
						m_MessageManager.Publish("startMusic", weeklyTracks[i]);
					};
					LimitedTextWrap(weeklyTracks[i].title.c_str(), width, 3);
				}

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Albums");
				ImGui::TableNextRow();
				for (size_t i = 0; i < 5; i++) {
					ImGui::TableNextColumn();
					if (ImGui::ImageButton(std::string("##weeklyA" + i).c_str(), m_ImageManager.GetImTexture(weeklyAlbums[i].id), imageSize)) {
						m_MessageManager.Publish("openAlbum", weeklyAlbums[i]);
						m_MessageManager.Publish("changeScene", std::string("AlbumScene"));
					}
					LimitedTextWrap(weeklyAlbums[i].name.c_str(), width, 3);
				}

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Artists");
				ImGui::TableNextRow();
				for (size_t i = 0; i < 5; i++) {
					ImGui::TableNextColumn();
					if (ImGui::ImageButton(std::string("##weeklyAr" + i).c_str(), m_ImageManager.GetImTexture(weeklyArtists[i].id), imageSize)) {
						m_MessageManager.Publish("openArtist", weeklyArtists[i]);
						m_MessageManager.Publish("changeScene", std::string("ArtistScene"));
					}
					LimitedTextWrap(weeklyArtists[i].artistName.c_str(), width, 3);
				}

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Monthly");
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Tracks");
				ImGui::TableNextRow();
				for (size_t i = 0; i < 5; i++) {
					ImGui::TableNextColumn();
					if (ImGui::ImageButton(std::string("##monthlyT" + i).c_str(), m_ImageManager.GetImTexture(monthlyTracks[i].id), imageSize)) {
						m_MessageManager.Publish("startMusic", monthlyTracks[i]);
					};
					LimitedTextWrap(monthlyTracks[i].title.c_str(), width, 3);
				}

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Albums");
				ImGui::TableNextRow();
				for (size_t i = 0; i < 5; i++) {
					ImGui::TableNextColumn();
					if (ImGui::ImageButton(std::string("##monthlyA" + i).c_str(), m_ImageManager.GetImTexture(monthlyAlbums[i].id), imageSize)) {
						m_MessageManager.Publish("openAlbum", monthlyAlbums[i]);
						m_MessageManager.Publish("changeScene", std::string("AlbumScene"));
					}
					LimitedTextWrap(monthlyAlbums[i].name.c_str(), width, 3);
				}

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Artists");
				ImGui::TableNextRow();
				for (size_t i = 0; i < 5; i++) {
					ImGui::TableNextColumn();
					if (ImGui::ImageButton(std::string("##monthlyAr" + i).c_str(), m_ImageManager.GetImTexture(monthlyArtists[i].id), imageSize)) {
						m_MessageManager.Publish("openArtist", monthlyArtists[i]);
						m_MessageManager.Publish("changeScene", std::string("ArtistScene"));
					}
					LimitedTextWrap(monthlyArtists[i].artistName.c_str(), width, 3);
				}
				ImGui::EndTable();
			}
			ImGui::PopStyleVar();
		
		}
		
		ImGui::PopStyleColor();
	}

	uint16_t PopularsModule::OnRender() {
		upProps &= BIT(0);
		ImGui::Begin(GetModuleName().c_str(), nullptr, defaultWindowFlags | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
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
		std::string weeklyTrack = "https://api.jamendo.com/v3.0/tracks/?client_id=" + std::string(CLIENTID) + "&format=jsonpretty&order=popularity_week&offset=1&limit=5";
		std::string weeklyAlbum = "https://api.jamendo.com/v3.0/albums/?client_id=" + std::string(CLIENTID) + "&format=jsonpretty&order=popularity_week&limit=5";
		std::string weeklyArtist = "https://api.jamendo.com/v3.0/artists/?client_id=" + std::string(CLIENTID) + "&format=jsonpretty&order=popularity_week&limit=5";
		std::string monthlyTrack = "https://api.jamendo.com/v3.0/tracks/?client_id=" + std::string(CLIENTID) + "&format=jsonpretty&order=popularity_month&offset=1&limit=5";
		std::string monthlyAlbum = "https://api.jamendo.com/v3.0/albums/?client_id=" + std::string(CLIENTID) + "&format=jsonpretty&order=popularity_month&limit=5";
		std::string monthlyArtist = "https://api.jamendo.com/v3.0/artists/?client_id=" + std::string(CLIENTID) + "&format=jsonpretty&order=popularity_month&limit=5";

		std::future<std::vector<Track>> futureWeeklyTracks = std::async(std::launch::async, [this, weeklyTrack]() { return m_NetworkManager.GetTrack(weeklyTrack); });
		std::future<std::vector<Album>> futureWeeklyAlbums = std::async(std::launch::async, [this, weeklyAlbum]() { return m_NetworkManager.GetAlbum(weeklyAlbum); });
		std::future<std::vector<Artist>> futureWeeklyArtists = std::async(std::launch::async, [this, weeklyArtist]() { return m_NetworkManager.GetArtist(weeklyArtist); });
		std::future<std::vector<Track>> futureMonthlyTracks = std::async(std::launch::async, [this, monthlyTrack]() { return m_NetworkManager.GetTrack(monthlyTrack); });
		std::future<std::vector<Album>> futureMonthlyAlbums = std::async(std::launch::async, [this, monthlyAlbum]() { return m_NetworkManager.GetAlbum(monthlyAlbum); });
		std::future<std::vector<Artist>> futureMonthlyArtists = std::async(std::launch::async, [this, monthlyArtist]() { return m_NetworkManager.GetArtist(monthlyArtist); });
		
		std::string weeklyDetailedAlbums = "https://api.jamendo.com/v3.0/tracks/?client_id=" + std::string(CLIENTID) + "&format=jsonpretty";
		std::string monthlyDetailedAlbums = "https://api.jamendo.com/v3.0/tracks/?client_id=" + std::string(CLIENTID) + "&format=jsonpretty";
		
		weeklyTracks = futureWeeklyTracks.get();
		weeklyAlbums = futureWeeklyAlbums.get();
		weeklyArtists = futureWeeklyArtists.get();
		monthlyTracks = futureMonthlyTracks.get();
		monthlyAlbums = futureMonthlyAlbums.get();
		monthlyArtists = futureMonthlyArtists.get();
		for (size_t i = 0; i < weeklyAlbums.size(); i++) {
			weeklyDetailedAlbums += "&album_id[]=" + std::to_string(weeklyAlbums[i].id);
		}
		for (size_t i = 0; i < monthlyAlbums.size(); i++) {
			monthlyDetailedAlbums += "&album_id[]=" + std::to_string(monthlyAlbums[i].id);
		}

		std::future<std::vector<Track>> futureDetailedWeeklyAlbums = std::async(std::launch::async, [this, weeklyDetailedAlbums]() { return m_NetworkManager.GetTrack(weeklyDetailedAlbums); });
		std::future<std::vector<Track>> futureDetailedMonthlyAlbums = std::async(std::launch::async, [this, monthlyDetailedAlbums]() { return m_NetworkManager.GetTrack(monthlyDetailedAlbums); });
		
		std::vector<Track> wAlbumTracks = futureDetailedWeeklyAlbums.get();
		std::vector<Track> mAlbumTracks = futureDetailedMonthlyAlbums.get();

		for (size_t i = 0; i < weeklyAlbums.size(); i++) {
			for (size_t j = 0; j < wAlbumTracks.size(); j++) {
				if (weeklyAlbums[i].id == wAlbumTracks[j].albumID) {
					if (weeklyAlbums[i].tracks.size() == 0) {
						weeklyAlbums[i].imageURL = wAlbumTracks[j].imageURL;
					}
					weeklyAlbums[i].tracks.push_back(wAlbumTracks[j]);
				}
			}
		}
		for (size_t i = 0; i < monthlyAlbums.size(); i++) {
			for (size_t j = 0; j < mAlbumTracks.size(); j++) {
				if (monthlyAlbums[i].id == mAlbumTracks[j].albumID) {
					if (monthlyAlbums[i].tracks.size() == 0) {
						monthlyAlbums[i].imageURL = mAlbumTracks[j].imageURL;
					}
					monthlyAlbums[i].tracks.push_back(mAlbumTracks[j]);
				}
			}
		}

	}
	
}