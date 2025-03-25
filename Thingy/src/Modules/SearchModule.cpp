#include "SearchModule.h"

namespace Thingy {
	void SearchModule::SetupSubscriptions() {
		m_MessageManager.Subscribe("openSearch", GetModuleName(), [this](MessageData data) {
			if (data.type() == typeid(std::pair<std::string, int>)) {
				std::pair<std::string, int> pair = std::any_cast<std::pair<std::string, int>>(data);
				std::string searchTerm = pair.first;
				whichToggled = pair.second;
				trackResults.clear();
				albumResults.clear();
				artistResults.clear();
				playlistResults.clear();
				switch (whichToggled) {
				case 0: { //all
					GetTrackResults(searchTerm);
					GetAlbumResults(searchTerm);
					GetArtistResults(searchTerm);
					GetPlaylistResults(searchTerm);
					break;
				}
				case 1: { //tags
					std::string url = "https://api.jamendo.com/v3.0/tracks/?client_id=" + std::string(CLIENTID) + "&format=jsonpretty&limit=100&tags=" + searchTerm;
					std::string response = m_NetworkManager.GetRequest(url);
					json parsed;
					try {
						parsed = json::parse(response);
					} catch (const nlohmann::json::parse_error& e) {
						T_ERROR("JSON parse error: {0}", e.what());
					}
					if (parsed["results"].empty()) {
						T_INFO("no results");
					} else {
						for (size_t i = 0; i < parsed["results"].size(); i++) {
							Track currTrack = parsed["results"][i];
							trackResults.push_back(currTrack);
						}
					}
					break;
				}
				case 2: { //tracks
					GetTrackResults(searchTerm);
					break;
				}
				case 3: { //albums
					GetAlbumResults(searchTerm);
					break;
				}
				case 4: { //artists
					GetArtistResults(searchTerm);
					break;
				}
				case 5: { //playlists
					GetPlaylistResults(searchTerm);
					break;
				}
				default:
					break;
				}
				LoadTextures();
			} else {
				T_ERROR("SearchModule: Invalid data type for openSearch");
			}
			});

		m_MessageManager.Subscribe("loggedIn", GetModuleName(), [this](const MessageData data) {
			if (data.type() == typeid(bool)) {
				loggedIn = std::any_cast<bool>(data);
			}
			});

		m_MessageManager.Subscribe("userChanged", GetModuleName(), [this](const MessageData data) {
			//TODO userChanged
			});
	}

	void SearchModule::LoadTextures() {
		imageFutures.clear();
		for (auto& track : trackResults) {
			if (!textures[track.id]) {
				std::string& url = track.imageURL;
				imageFutures.emplace(track.id, std::async(std::launch::async, [this, &url]() { return m_ImageManager.GetImage(url); }));
			}
		}
		for (auto& album : albumResults) {
			if (!textures[album.id]) {
				std::string& url = album.imageURL;
				imageFutures.emplace(album.id, std::async(std::launch::async, [this, &url]() { return m_ImageManager.GetImage(url); }));
			}
		}
		for (auto& artist : artistResults) {
			if (!textures[artist.id]) {
				if (artist.artistImageURL == "") {
					textures[artist.id] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetDefaultArtistImage());
				} else {
					std::string& url = artist.artistImageURL;
					imageFutures.emplace(artist.id, std::async(std::launch::async, [this, &url]() { return m_ImageManager.GetImage(url); }));
				}
			}
		}
		for (auto& playlist : playlistResults) {
			if (!textures[playlist.playlistID]) {
				if (playlist.playlistCoverBuffer.empty()) {
					textures[playlist.playlistID] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetDefaultPlaylistImage());
				} else {
					textures[playlist.playlistID] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTextureFromImage(Image(playlist.playlistCoverBuffer)));
				}
			}
		}
	}
	
	void SearchModule::OnLoad(const std::variant<int, std::string> moduleState) {}

	void SearchModule::OnUpdate() {
		if (!imageFutures.empty()) {
			for (auto it = imageFutures.begin(); it != imageFutures.end(); ) {
				auto& image = it->second;
				if (image.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
					textures[it->first] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTextureFromImage(image.get()));
					it = imageFutures.erase(it);
				} else {
					++it;
				}
			}
		}
	}

	void SearchModule::Window() {
		ImVec2 barSize = ImVec2(GetSize().x - 30, 30);
		DragBar(upProps, barSize);
		//TODO: Show search results.
		switch (whichToggled) {
		case 0: { //all
			AllResultsDisplay();
			break;
		}
		case 1: { //tags
			TrackResultsDisplay();
			break;
		}
		case 2: { //tracks
			TrackResultsDisplay();
			break;
		}
		case 3: { //albums
			AlbumResultsDisplay();
			break;
		}
		case 4: { //artists
			ArtistResultsDisplay();
			break;
		}
		case 5: { //playlists
			PlaylistResultsDisplay();
			break;
		}
		default:
			break;
		}
	}

	uint16_t SearchModule::OnRender() {
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
		
		upProps &= BIT(0);
		ImGui::Begin(GetModuleName().c_str(), nullptr, defaultWindowFlags  |( whichToggled != 0 ? flags : 0));
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

	void SearchModule::AllResultsDisplay() {
		if (!trackResults.empty()) {
			ImGui::Text("Tracks");
			ImGui::BeginChild("Tracks", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);
			for (size_t i = 0; i < trackResults.size(); i++) {
				ImGui::BeginGroup();
				ImGui::Image(reinterpret_cast<ImTextureID>(textures[trackResults[i].id].get()), { 200.0f, 200.0f });
				if (ImGui::IsItemClicked()) {
				};
				LimitedTextWrap(trackResults[i].title.c_str(), 180, 3);
				ImGui::EndGroup();
				ImGui::SameLine();
			}
			ImGui::EndChild();
		}

		if (!albumResults.empty()) {
			ImGui::Text("Albums");
			ImGui::BeginChild("Albums", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);
			for (size_t i = 0; i < albumResults.size(); i++) {
				ImGui::BeginGroup();
				ImGui::Image(reinterpret_cast<ImTextureID>(textures[albumResults[i].id].get()), {200.0f, 200.0f});
				if (ImGui::IsItemClicked()) {
				};
				LimitedTextWrap(albumResults[i].name.c_str(), 180, 3);
				ImGui::EndGroup();
				ImGui::SameLine();
			}
			ImGui::EndChild();
		}

		if (!artistResults.empty()) {
			ImGui::Text("Artists");
			ImGui::BeginChild("artists", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);
			for (size_t i = 0; i < artistResults.size(); i++) {
				ImGui::BeginGroup();
				ImGui::Image(reinterpret_cast<ImTextureID>(textures[artistResults[i].id].get()), { 200.0f, 200.0f });
				if (ImGui::IsItemClicked()) {
				};
				LimitedTextWrap(artistResults[i].artistName.c_str(), 180, 3);
				ImGui::EndGroup();
				ImGui::SameLine();
			}
			ImGui::EndChild();
		}

		if (!playlistResults.empty()) {
			ImGui::Text("Playlists");
			ImGui::BeginChild("playlists", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);
			for (size_t i = 0; i < playlistResults.size(); i++) {
				ImGui::BeginGroup();
				ImGui::Image(reinterpret_cast<ImTextureID>(textures[playlistResults[i].playlistID].get()), { 200.0f, 200.0f });
				if (ImGui::IsItemClicked()) {
				};
				LimitedTextWrap(playlistResults[i].playlistName.c_str(), 180, 3);
				ImGui::EndGroup();
				ImGui::SameLine();
			}
			ImGui::EndChild();
		}
		if (trackResults.empty() && albumResults.empty() && artistResults.empty() && playlistResults.empty()) {
			ImGui::Text("No results :c");
		}
	}

	void SearchModule::TrackResultsDisplay() {
		if (trackResults.empty()) {
			ImGui::Text("No results :c");
		} else {

			if (ImGui::ImageButton("##highlighted", reinterpret_cast<ImTextureID>(textures[trackResults[0].id].get()), ImVec2(200.0f, 200.0f))) {
				m_MessageManager.Publish("startMusic", trackResults[0]);
			};
			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::Text(U8(trackResults[0].title.c_str()));
			ImGui::Text(U8(trackResults[0].albumName.c_str()));
			if (ImGui::IsItemHovered()) {
				upProps |= BIT(3);
			}
			if (ImGui::IsItemClicked()) {
				m_MessageManager.Publish("openAlbum", trackResults[0].albumID);
				m_MessageManager.Publish("changeScene", std::string("AlbumScene"));
			}
			ImGui::Text(U8(trackResults[0].artistName.c_str()));
			if (ImGui::IsItemHovered()) {
				upProps |= BIT(3);
			}
			if (ImGui::IsItemClicked()) {
				m_MessageManager.Publish("openArtist", trackResults[0].artistID);
				m_MessageManager.Publish("changeScene", std::string("ArtistScene"));
			}

			ImGui::EndGroup();
			if (trackResults.size() > 1) {
				if(ImGui::BeginTable("trackDisplay",2, ImGuiTableFlags_ScrollY, ImVec2(ImGui::GetWindowSize().x - 10.0f, ImGui::GetWindowSize().y - 250.0f))) {
					ImGui::TableSetupColumn("Fixed", ImGuiTableColumnFlags_WidthFixed, 120.0f);
					ImGui::TableSetupColumn("Auto");
					for (size_t i = 1; i < trackResults.size(); i++) {
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						std::string label = "##" + std::to_string(i);
						if (ImGui::ImageButton(label.c_str(), reinterpret_cast<ImTextureID>(textures[trackResults[i].id].get()), ImVec2(100.0f, 100.0f))) {
							m_MessageManager.Publish("startMusic", trackResults[i]);
						};
						ImGui::TableSetColumnIndex(1);
						ImGui::Text(U8(trackResults[i].title.c_str()));
						ImGui::Text(U8(trackResults[i].albumName.c_str()));
						if (ImGui::IsItemHovered()) {
							upProps |= BIT(3);
						}
						if (ImGui::IsItemClicked()) {
							m_MessageManager.Publish("openAlbum", trackResults[i].albumID);
							m_MessageManager.Publish("changeScene", std::string("AlbumScene"));
						}
						ImGui::Text(U8(trackResults[i].artistName.c_str()));
						if (ImGui::IsItemHovered()) {
							upProps |= BIT(3);
						}
						if (ImGui::IsItemClicked()) {
							m_MessageManager.Publish("openArtist", trackResults[i].artistID);
							m_MessageManager.Publish("changeScene", std::string("ArtistScene"));
						}
					}
					ImGui::EndTable();
				};
			}
		}
	}

	void SearchModule::AlbumResultsDisplay() {
		if (albumResults.empty()) {
			ImGui::Text("No results :c");
		} else {

			if (ImGui::ImageButton("##highlighted", reinterpret_cast<ImTextureID>(textures[albumResults[0].id].get()), ImVec2(200.0f, 200.0f))) {
				m_MessageManager.Publish("openAlbum", albumResults[0]);
				m_MessageManager.Publish("changeScene", std::string("AlbumScene"));
			};
			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::Text(U8(albumResults[0].name.c_str()));
			ImGui::Text(U8(albumResults[0].artistName.c_str()));
			if (ImGui::IsItemHovered()) {
				upProps |= BIT(3);
			}
			if (ImGui::IsItemClicked()) {
				m_MessageManager.Publish("openArtist", albumResults[0].artistID);
				m_MessageManager.Publish("changeScene", std::string("ArtistScene"));
			}

			ImGui::EndGroup();
			if (albumResults.size() > 1) {
				if (ImGui::BeginTable("albumDisplay", 2, ImGuiTableFlags_ScrollY, ImVec2(ImGui::GetWindowSize().x - 10.0f, ImGui::GetWindowSize().y - 250.0f))) {
					ImGui::TableSetupColumn("Fixed", ImGuiTableColumnFlags_WidthFixed, 120.0f);
					ImGui::TableSetupColumn("Auto");
					for (size_t i = 1; i < albumResults.size(); i++) {
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						std::string label = "##" + std::to_string(i);
						if (ImGui::ImageButton(label.c_str(), reinterpret_cast<ImTextureID>(textures[albumResults[i].id].get()), ImVec2(100.0f, 100.0f))) {
							m_MessageManager.Publish("openAlbum", albumResults[i]);
							m_MessageManager.Publish("changeScene", std::string("AlbumScene"));
						};
						ImGui::TableSetColumnIndex(1);
						ImGui::Text(U8(albumResults[i].name.c_str()));
						ImGui::Text(U8(albumResults[i].artistName.c_str()));
						if (ImGui::IsItemHovered()) {
							upProps |= BIT(3);
						}
						if (ImGui::IsItemClicked()) {
							m_MessageManager.Publish("openArtist", albumResults[i].artistID);
							m_MessageManager.Publish("changeScene", std::string("ArtistScene"));
						}
					}
					ImGui::EndTable();
				};
			}
		}
	}

	void SearchModule::ArtistResultsDisplay() {
		if (artistResults.empty()) {
			ImGui::Text("No results :c");
		} else {

			if (ImGui::ImageButton("##highlighted", reinterpret_cast<ImTextureID>(textures[artistResults[0].id].get()), ImVec2(200.0f, 200.0f))) {
				m_MessageManager.Publish("openArtist", artistResults[0].id);
				m_MessageManager.Publish("changeScene", std::string("ArtistScene"));
			};
			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::Text(U8(artistResults[0].artistName.c_str()));
			if (ImGui::IsItemHovered()) {
				upProps |= BIT(3);
			}
			if (ImGui::IsItemClicked()) {
				m_MessageManager.Publish("openArtist", artistResults[0].id);
				m_MessageManager.Publish("changeScene", std::string("ArtistScene"));
			}

			ImGui::EndGroup();
			if (artistResults.size() > 1) {
				if (ImGui::BeginTable("artistDisplay", 2, ImGuiTableFlags_ScrollY, ImVec2(ImGui::GetWindowSize().x - 10.0f, ImGui::GetWindowSize().y - 250.0f))) {
					ImGui::TableSetupColumn("Fixed", ImGuiTableColumnFlags_WidthFixed, 120.0f);
					ImGui::TableSetupColumn("Auto");
					for (size_t i = 1; i < artistResults.size(); i++) {
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						std::string label = "##" + std::to_string(i);
						if (ImGui::ImageButton(label.c_str(), reinterpret_cast<ImTextureID>(textures[artistResults[i].id].get()), ImVec2(100.0f, 100.0f))) {
							m_MessageManager.Publish("openArtist", artistResults[i].id);
							m_MessageManager.Publish("changeScene", std::string("ArtistScene"));
						};
						ImGui::TableSetColumnIndex(1);
						ImGui::Text(U8(artistResults[i].artistName.c_str()));
						if (ImGui::IsItemHovered()) {
							upProps |= BIT(3);
						}
						if (ImGui::IsItemClicked()) {
							m_MessageManager.Publish("openArtist", artistResults[i].id);
							m_MessageManager.Publish("changeScene", std::string("ArtistScene"));
						}
					}
					ImGui::EndTable();
				};
			}
		}
	}

	void SearchModule::PlaylistResultsDisplay() {
		if (playlistResults.empty()) {
			ImGui::Text("No results :c");
		} else {

			if (ImGui::ImageButton("##highlighted", reinterpret_cast<ImTextureID>(textures[playlistResults[0].playlistID].get()), ImVec2(200.0f, 200.0f))) {
				m_MessageManager.Publish("openPlaylist", playlistResults[0]);
				m_MessageManager.Publish("changeScene", std::string("PlaylistScene"));
			};
			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::Text(U8(playlistResults[0].playlistName.c_str()));
			if (ImGui::IsItemHovered()) {
				upProps |= BIT(3);
			}
			if (ImGui::IsItemClicked()) {
				m_MessageManager.Publish("openPlaylist", playlistResults[0].playlistID);
				m_MessageManager.Publish("changeScene", std::string("PlaylistScene"));
			}
			ImGui::Text(U8(playlistResults[0].playlistName.c_str()));

			ImGui::EndGroup();
			if (playlistResults.size() > 1) {
				if (ImGui::BeginTable("playlistDisplay", 2, ImGuiTableFlags_ScrollY, ImVec2(ImGui::GetWindowSize().x - 10.0f, ImGui::GetWindowSize().y - 250.0f))) {
					ImGui::TableSetupColumn("Fixed", ImGuiTableColumnFlags_WidthFixed, 120.0f);
					ImGui::TableSetupColumn("Auto");
					for (size_t i = 1; i < playlistResults.size(); i++) {
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						std::string label = "##" + std::to_string(i);
						if (ImGui::ImageButton(label.c_str(), reinterpret_cast<ImTextureID>(textures[playlistResults[i].playlistID].get()), ImVec2(100.0f, 100.0f))) {
							m_MessageManager.Publish("openPlaylist", playlistResults[i].playlistID);
							m_MessageManager.Publish("changeScene", std::string("PlaylistScene"));
						};
						ImGui::TableSetColumnIndex(1);
						ImGui::Text(U8(playlistResults[i].playlistName.c_str()));
						if (ImGui::IsItemHovered()) {
							upProps |= BIT(3);
						}
						if (ImGui::IsItemClicked()) {
							m_MessageManager.Publish("openPlaylist", playlistResults[i]);
							m_MessageManager.Publish("changeScene", std::string("PlaylistScene"));
						}
					}
					ImGui::EndTable();
				};
			}
		}
	}

	void SearchModule::GetTrackResults(std::string term) {
		std::string url = "https://api.jamendo.com/v3.0/tracks/?client_id="
			+ std::string(CLIENTID)
			+ "&format=jsonpretty&limit=100&search="
			+ term;
		std::string response = m_NetworkManager.GetRequest(url);
		json parsed;
		try {
			parsed = json::parse(response);
		} catch (const nlohmann::json::parse_error& e) {
			T_ERROR("JSON parse error: {0}", e.what());
		}
		if (parsed["results"].empty()) {
			T_INFO("no results");
		} else {
			for (size_t i = 0; i < parsed["results"].size(); i++) {
				Track currTrack = parsed["results"][i];
				trackResults.push_back(currTrack);
			}
		}
	}

	void SearchModule::GetAlbumResults(std::string term) {
		std::string url = "https://api.jamendo.com/v3.0/albums/?client_id="
			+ std::string(CLIENTID)
			+ "&format=jsonpretty&limit=100&namesearch="
			+ term;
		std::string response = m_NetworkManager.GetRequest(url);
		json parsed;
		try {
			parsed = json::parse(response);
		} catch (const nlohmann::json::parse_error& e) {
			T_ERROR("JSON parse error: {0}", e.what());
		}
		if (parsed["results"].empty()) {
			T_INFO("no results");
		} else {
			std::unordered_set<int> albumIDs;
			for (size_t i = 0; i < parsed["results"].size(); i++) {
				albumIDs.insert(std::stoi(parsed["results"][i]["id"].get<std::string>()));
			}
			std::string url2 = "https://api.jamendo.com/v3.0/artists/tracks/?client_id="
				+ std::string(CLIENTID)
				+ "&format=jsonpretty&";
			for (auto& albumID : albumIDs) {
				url2 += "&album_id[]=" + std::to_string(albumID);
			}
			std::vector<Artist> artists = m_NetworkManager.GetArtistsWithTracks(url2);
			for (auto& artist : artists) {
				albumResults.insert(albumResults.end(), artist.albums.begin(), artist.albums.end());
			}
		}
	}
	
	void SearchModule::GetArtistResults(std::string term) {
		std::string url = "https://api.jamendo.com/v3.0/artists/?client_id="
			+ std::string(CLIENTID)
			+ "&format=jsonpretty&limit=100&namesearch="
			+ term;
		std::string response = m_NetworkManager.GetRequest(url);
		json parsed;
		try {
			parsed = json::parse(response);
		} catch (const nlohmann::json::parse_error& e) {
			T_ERROR("JSON parse error: {0}", e.what());
		}
		if (parsed["results"].empty()) {
			T_INFO("no results");
		} else {
			for (size_t i = 0; i < parsed["results"].size(); i++) {
				Artist currArtist = parsed["results"][i];
				artistResults.push_back(currArtist);
			}
		}
	}
	
	void SearchModule::GetPlaylistResults(std::string term) {
		std::string url = "http://localhost:3000/search/playlist/?term="
			+ term;

		std::string response = m_NetworkManager.GetRequest(url);
		json parsed;
		try {
			parsed = json::parse(response);
		} catch (const nlohmann::json::parse_error& e) {
			T_ERROR("JSON parse error: {0}", e.what());
		}
		if (parsed.empty() || parsed.contains("message")) {
			T_INFO("no results");
		} else {
			T_INFO("{0}", parsed.dump());
			for (size_t i = 0; i < parsed.size(); i++) {

				Playlist currPlaylist = parsed[i];
				playlistResults.push_back(currPlaylist);
			}
		}
	}
}