#include "AlbumModule.h"


namespace Thingy {

	void AlbumModule::SetupSubscriptions() {
		m_MessageManager.Subscribe("openAlbum", GetModuleName(), [this](const MessageData data) {
			
				if (data.type() == typeid(Album)) {
					Album recAlbum = std::any_cast<Album>(data);
					for (size_t i = 0; i < album.size(); i++) {
						if (album[i].name == recAlbum.name) {
							curr = i;
							T_INFO("returned");
							return;
						}
					}
					if (recAlbum.tracks.empty()) {
						recAlbum.tracks = m_NetworkManager.GetTrack("https://api.jamendo.com/v3.0/tracks/?client_id=" + std::string(CLIENTID) + "&format=jsonpretty&limit=200&album_id=2442");
					}
					album.emplace_back(recAlbum);
					curr = album.size() - 1;
					T_INFO("{0}", album[curr].toString());
				} else {
					T_ERROR("AlbumModule: Invalid data type for openAlbum");
				}
			});

		m_MessageManager.Subscribe("beforeSwitch" + GetModuleName(), GetModuleName(), [this](const MessageData data) {
			m_MessageManager.Publish("saveModuleState", std::make_pair<std::string, std::variant<int, std::string>>(GetModuleName(), album[curr].name));
			});

		m_MessageManager.Subscribe("loggedIn", GetModuleName(), [this](const MessageData data) {
			if (data.type() == typeid(bool)) {
				loggedIn = std::any_cast<bool>(data);
			}
			});

		m_MessageManager.Subscribe("userChanged", GetModuleName(), [this](const MessageData data) {
			UserInfoChanged();
			});
		
	}

	void AlbumModule::OnLoad(const std::variant<int, std::string> moduleState) {
		if (std::holds_alternative<std::string>(moduleState)) {
			std::string albumName = std::get<std::string>(moduleState);
			for (size_t i = 0; i < album.size(); i++) {
				if (album[i].name == albumName) {
					curr = i;
				}
			}
		}
		if (std::holds_alternative<int>(moduleState)) {
			T_ERROR("album got: {0}", std::get<int>(moduleState));
		}
		if (!textures[album[curr].id]) 
			textures[album[curr].id] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTexture(album[curr].imageURL));

		length = 0;
		for (auto& track : album[curr].tracks) {
			length += track.duration;
		}
	}
	
	void AlbumModule::OnUpdate() {
	}
	
	void AlbumModule::Window() {

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

		ImGui::Image(reinterpret_cast<ImTextureID>(textures[album[curr].id].get()), {300.0f, 300.0f});
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::Text(U8(album[curr].name.c_str()));
		ImGui::Text(U8(album[curr].artistName.c_str()));
		if (ImGui::IsItemHovered()) {
			upProps |= BIT(3);
		}
		if (ImGui::IsItemClicked()) {
			m_MessageManager.Publish("openArtist", album[curr].artistID);
			m_MessageManager.Publish("changeScene", std::string("ArtistScene"));
		}
		ImGui::Text("Release Date: %s", album[curr].releaseDate.c_str());
		ImGui::Text("Track count: %zu", album[curr].tracks.size());
		ImGui::SameLine();
		ImGui::Text("Album length: %s", SecondsToTimeString(length).c_str());
		if (loggedIn) {
			if (ImGui::Button("Add to Playlist")) {
				selectedPlaylists.clear();
				ImGui::OpenPopup("Add to playlists");
			}
		}
		ImGui::EndGroup();
		ImGui::BeginChild("Tracks", ImVec2(0,300), false, ImGuiWindowFlags_HorizontalScrollbar);
		for (size_t i = 0; i < album[curr].tracks.size(); i++) {
			Track& track = album[curr].tracks[i];
			ImGui::BeginGroup();
			ImGui::Image(reinterpret_cast<ImTextureID>(textures[album[curr].id].get()), { 200.0f, 200.0f });
			if (ImGui::IsItemClicked()) {
				std::vector<Track> tracks(album[curr].tracks.begin() + i, album[curr].tracks.end());
				std::vector<Track> history(album[curr].tracks.begin(), album[curr].tracks.begin() + i);
				m_AudioManager.ClearQueue();
				m_MessageManager.Publish("newHistory", history);
				m_MessageManager.Publish("addToQueue", tracks);
				m_MessageManager.Publish("startMusic", "");
			};
			LimitedTextWrap(track.title.c_str(), 180, 3);
			ImGui::EndGroup();
			ImGui::SameLine();
		}
		ImGui::EndChild();

		PlaylistModal();
		
	}

	uint16_t AlbumModule::OnRender() {
		upProps &= BIT(0);
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
		return upProps;
	}

	void AlbumModule::PlaylistModal() {

		ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImVec2 modalSize = ImVec2(310.0f, 600.0f);
		ImVec2 modalPos = ImVec2(center.x - modalSize.x * 0.5f, center.y - modalSize.y * 0.5f);
		ImGui::SetNextWindowPos(modalPos);
		ImGui::SetNextWindowSize(modalSize);
		if (ImGui::BeginPopupModal("Add to playlists", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
			if (ImGui::BeginTable("AddToPlaylistsTable", 3, ImGuiTableFlags_ScrollY, ImVec2(300.0f, 500.0f))) {
				ImGui::TableSetupColumn("Cover", ImGuiTableColumnFlags_WidthFixed, 80.0f);
				ImGui::TableSetupColumn("Info", ImGuiTableColumnFlags_WidthFixed, 150.0f);
				ImGui::TableSetupColumn("Check");

				int i = 0;
				for (auto& playlist : user.playlists) {
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Image(reinterpret_cast<ImTextureID>(playlistTextures[playlist.playlistID].get()), { 80.0f, 80.0f });
					ImGui::TableSetColumnIndex(1);
					ImGui::SameLine();
					ImGui::BeginGroup();
					ImGui::Text(U8(playlist.playlistName.c_str()));
					ImGui::TableSetColumnIndex(2);
					ImGui::Checkbox(std::string("##" + std::to_string(playlist.playlistID)).c_str(), &selectedPlaylists[playlist.playlistID]);

					ImGui::EndGroup();
					i++;
				}

				ImGui::EndTable();
				if (ImGui::Button("done")) {
					std::string url = "http://localhost:3000/playlists/add?playlistIds=";
					bool hasSelected = false;
					for (auto& playlist : user.playlists) {
						if (selectedPlaylists[playlist.playlistID]) {
							hasSelected = true;
							url += std::to_string(playlist.playlistID) + ",";
						}
					}
					if (hasSelected) {
						url.pop_back();
					}
					for (auto& track : album[curr].tracks) {
						url += "&trackId=" + std::to_string(track.id);
					}
					T_INFO("{0}", url);
					if (hasSelected) {
						std::string token;
						m_AuthManager.RetrieveToken("accessToken", token);
						std::string json;
						T_INFO("{0}", m_NetworkManager.PostRequestAuth(url, json, token));
						m_MessageManager.Publish("updateUser", "");
					}
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}

	void AlbumModule::UserInfoChanged() {
		for (size_t i = 0; i < user.playlists.size(); i++) {
			const Playlist& currP = user.playlists[i];
			if (currP.playlistCoverBuffer.empty()) {
				playlistTextures[currP.playlistID] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetDefaultPlaylistImage());
			} else {
				playlistTextures[currP.playlistID] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTextureFromImage(Image(currP.playlistCoverBuffer)));
			};
		}
	}
}