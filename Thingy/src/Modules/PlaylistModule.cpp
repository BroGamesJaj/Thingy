#include "PlaylistModule.h"


namespace Thingy {

	void PlaylistModule::SetupSubscriptions() {
		m_MessageManager.Subscribe("openPlaylist", GetModuleName(), [this](const MessageData data) {

			if (data.type() == typeid(Playlist)) {
				Playlist playlist = std::any_cast<Playlist>(data);
				for (size_t i = 0; i < playlists.size(); i++) {
					if (playlists[i].playlistID == playlist.playlistID) {
						curr = i;
						if (playlists[curr].playlistCoverBuffer.empty())
							m_ImageManager.AddPlaylistTexture(playlists[curr].playlistID, m_ImageManager.GetDefaultPlaylistImage());
						else m_ImageManager.AddPlaylistTexture(playlists[curr].playlistID, m_ImageManager.GetTextureFromImage(Image(playlists[curr].playlistCoverBuffer)));
						playlists[curr] = playlist;
						
						std::string url = "https://api.jamendo.com/v3.0/tracks/?client_id=" + std::string(CLIENTID) + "&format=jsonpretty&limit=200";
						bool needsUpdate = false;
						for (auto& trackId : playlist.trackIDs) {
							if (tracks.find(trackId) == tracks.end()) {
								url += "&id[]=" + std::to_string(trackId);
								needsUpdate = true;
							}
						}
						if (needsUpdate) {
							std::vector<Track> newTracks = m_NetworkManager.GetTrack(url);
							for (auto& newTrack : newTracks) {
								tracks[newTrack.id] = newTrack;
							}
						}
						std::unordered_map<uint32_t, std::future<Image>> images;
						length[curr] = 0;
						for (auto& trackId : playlists[curr].trackIDs) {
							if (!m_ImageManager.HasTextureAt(trackId)) {
								std::string& url = tracks[trackId].imageURL;
								images.emplace(trackId, std::async(std::launch::async, [this, &url]() { return m_ImageManager.GetImage(url); }));
							}
							length[curr] += tracks[trackId].duration;
						}
						while (!images.empty()) {
							for (auto it = images.begin(); it != images.end(); ) {
								auto& image = it->second;
								if (image.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
									m_ImageManager.AddTexture(it->first, m_ImageManager.GetTextureFromImage(image.get()));
									it = images.erase(it);
								}
								else {
									++it;
								}
							}
						}
						T_INFO("returned");
						return;
					}
				}
				std::string url = "https://api.jamendo.com/v3.0/artists/tracks/?client_id=" + std::string(CLIENTID) + "&format=jsonpretty&limit=200";
				bool needsUpdate = false;
				for (auto& trackId : playlist.trackIDs) {
					if (tracks.find(trackId) == tracks.end()) {
						url += "&track_id[]=" + std::to_string(trackId);
						needsUpdate = true;
					}
				}
				if (needsUpdate) {
					std::vector<Track> newTracks = m_NetworkManager.GetTracksFromArtist(url);
					for (auto& newTrack : newTracks) {
						tracks[newTrack.id] = newTrack;
					}
				}
				playlists.push_back(playlist);
				curr = playlists.size() - 1;
				if (playlists[curr].playlistCoverBuffer.empty())
					m_ImageManager.AddPlaylistTexture(playlists[curr].playlistID, m_ImageManager.GetDefaultPlaylistImage());
				else m_ImageManager.AddPlaylistTexture(playlists[curr].playlistID, m_ImageManager.GetTextureFromImage(Image(playlists[curr].playlistCoverBuffer)));
			
				std::unordered_map<uint32_t, std::future<Image>> images;
				length.push_back(0);
				for (auto& trackId : playlists[curr].trackIDs) {
					if (!m_ImageManager.HasTextureAt(trackId)) {
						std::string& url = tracks[trackId].imageURL;
						images.emplace(trackId, std::async(std::launch::async, [this, &url]() { return m_ImageManager.GetImage(url); }));
					}
					length[curr] += tracks[trackId].duration;
				}
				while (!images.empty()) {
					for (auto it = images.begin(); it != images.end(); ) {
						auto& image = it->second;
						if (image.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
							m_ImageManager.AddTexture(it->first, m_ImageManager.GetTextureFromImage(image.get()));
							it = images.erase(it);
						} else {
							++it;
						}
					}
				}
			} else {
				T_ERROR("PlaylistModule: Invalid data type for openPlaylist");
			}
			});

		m_MessageManager.Subscribe("beforeSwitch" + GetModuleName(), GetModuleName(), [this](const MessageData data) {
			m_MessageManager.Publish("saveModuleState", std::make_pair<std::string, std::variant<int, std::string>>(GetModuleName(), playlists[curr].playlistName));
			});

		m_MessageManager.Subscribe("loggedIn", GetModuleName(), [this](const MessageData data) {
			if (data.type() == typeid(bool)) {
				loggedIn = std::any_cast<bool>(data);
			}
			});

		m_MessageManager.Subscribe("userChanged", GetModuleName(), [this](const MessageData data) {
			
			});

	}

	void PlaylistModule::OnLoad(const std::variant<int, std::string> moduleState) {
		if (std::holds_alternative<std::string>(moduleState)) {
			std::string playlistName = std::get<std::string>(moduleState);
			for (size_t i = 0; i < playlists.size(); i++) {
				if (playlists[i].playlistName == playlistName) {
					curr = i;
				}
			}
		}
		if (std::holds_alternative<int>(moduleState)) {
			T_ERROR("Playlist got: {0}", std::get<int>(moduleState));
		}

	}

	void PlaylistModule::OnUpdate() {}

	void PlaylistModule::Window() {
		ImVec2 barSize = ImVec2(GetSize().x - 20, 30);
		DragBar(upProps, barSize);

		ImGui::Image(m_ImageManager.GetPlaylistImTexture(playlists[curr].playlistID), {300.0f, 300.0f});
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::Text(U8(playlists[curr].playlistName.c_str()));
		ImGui::Text(U8(playlists[curr].description.c_str()));
		ImGui::Text("Track count: %zu", playlists[curr].trackIDs.size());
		ImGui::SameLine();
		ImGui::Text("Playlist length: %s", SecondsToTimeString(length[curr]).c_str());
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		if (loggedIn && playlists[curr].ownerID == user.userID && playlists[curr].playlistName != "Liked") {
			if (ImGui::Button("Edit playlist")) {
				error = "";
				editedDescription = playlists[curr].description;
				editedPlaylistName = playlists[curr].playlistName;
				editedIsPrivate = playlists[curr].priv;
				editedCoverPath = "";
				editedPlaylistCover = nullptr;
				ImGui::OpenPopup("Edit playlist");
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete playlist")) {
				ImGui::OpenPopup("Delete playlist");
			}
		} else if(playlists[curr].ownerID != user.userID && loggedIn) {
			auto it = std::find_if(user.followed.begin(), user.followed.end(), [&](const std::tuple<int, FollowedType, int>& tuple) {
				return std::get<0>(tuple) == playlists[curr].playlistID;
				});
			if (it != user.followed.end()) {
				if (ImGui::Button("Unfollow")) {
					std::string url = "http://localhost:3000/followed/" + std::to_string(std::get<2>(*it));
					std::string token;
					m_AuthManager.RetrieveToken("accessToken", token);

					m_NetworkManager.DeleteRequest(url, token);
					m_MessageManager.Publish("updateUser", "");
				}
			} else {
				if (ImGui::Button("Follow")) {
					std::string url = "http://localhost:3000/followed";
					std::string token;
					m_AuthManager.RetrieveToken("accessToken", token);
					char buffer[100];
					snprintf(buffer, sizeof(buffer), R"({"FollowedID": %d, "Type": "Playlist"})", playlists[curr].playlistID);
					std::string data = buffer;
					m_NetworkManager.PostRequestAuth(url, data, token);
					m_MessageManager.Publish("updateUser", "");
				}
			}
		}
		ImGui::PopStyleColor();
		ImGui::EndGroup();
		ImGui::BeginChild("Tracks", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);
		
		for (size_t i = 0; i < playlists[curr].trackIDs.size(); i++) {
			int& trackId = playlists[curr].trackIDs[i];
			ImGui::BeginGroup();
			ImGui::Image(m_ImageManager.GetImTexture(trackId), {200.0f, 200.0f});
			if (ImGui::IsItemClicked()) {
				std::vector<Track> queueTracks;
				for (size_t j = i; j < playlists[curr].trackIDs.size(); j++) {
					queueTracks.push_back(tracks[playlists[curr].trackIDs[j]]);
				}
				std::vector<Track> history;
				for (size_t j = 0; j < i; j++) {
					history.push_back(tracks[playlists[curr].trackIDs[j]]);
				}
				m_AudioManager.ClearQueue();
				m_MessageManager.Publish("newHistory", history);
				m_MessageManager.Publish("addToQueue", queueTracks);
				m_MessageManager.Publish("startMusic", "");
			};
			LimitedTextWrap(tracks[trackId].title.c_str(), 180, 3);
			ImGui::EndGroup();
			ImGui::SameLine();
		}
		
		ImGui::EndChild();

		EditPlaylistModal();
		DeletePlaylistModal();
	}

	uint16_t PlaylistModule::OnRender() {
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

	void PlaylistModule::EditPlaylistModal() {
		ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImVec2 modalSize = ImVec2(310.0f, 600.0f);
		ImVec2 modalPos = ImVec2(center.x - modalSize.x * 0.5f, center.y - modalSize.y * 0.5f);
		ImGui::SetNextWindowPos(modalPos);
		ImGui::SetNextWindowSize(modalSize);

		if (ImGui::BeginPopupModal("Edit playlist", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
			ImGui::SameLine(ImGui::GetWindowWidth() - 20);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
			if (ImGui::Button("X")) ImGui::CloseCurrentPopup();
			ImGui::PopStyleColor();
			ImGui::Image((editedPlaylistCover == nullptr ? m_ImageManager.GetImTexture(playlists[curr].playlistID) : reinterpret_cast<ImTextureID>(editedPlaylistCover.get())), ImVec2(100.0f, 100.0f));
			if (ImGui::IsItemHovered()) {
				upProps |= BIT(3);
			}
			if (ImGui::IsItemClicked()) {
				std::string imagePath;
				if (!OpenFileExplorer(imagePath)) {
					T_ERROR("OpenFileExplorer failed!");
				} else {
					if (IsImageFile(imagePath)) {
						editedCoverPath = imagePath;
						editedPlaylistCover = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTextureFromFile(editedCoverPath.c_str()));

					} else {
						T_INFO("not image path: {0}", imagePath);
					}
				}

			}
			ImGui::InputText("Playlist Name", &editedPlaylistName, 0, ResizeCallback, (void*)&editedPlaylistName);
			ImGui::InputText("Description", &editedDescription, 0, ResizeCallback, (void*)&editedDescription);
			ImGui::Checkbox("Private?", &editedIsPrivate);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
			if (ImGui::Button("Save Changes")) {
				if (editedPlaylistName.size() < 3) {
					error = "Playlist name cannot be shorter than 3 characters!";
				} else {
					std::string url = "http://localhost:3000/playlists/" + std::to_string(playlists[curr].playlistID);
					std::string token;
					m_AuthManager.RetrieveToken("accessToken", token);
					m_NetworkManager.UpdatePlaylist(url, editedPlaylistName, editedDescription, editedIsPrivate, editedCoverPath, token);
					m_MessageManager.Publish("updateUser", "");
					auto it = std::find_if(user.playlists.begin(), user.playlists.end(), [&](const Playlist& playlist) {
						return playlist.playlistID == playlists[curr].playlistID;
						});
					if (it != user.playlists.end()) {
						playlists[curr] = *it;
						if (playlists[curr].playlistCoverBuffer.empty())
							m_ImageManager.AddTexture(playlists[curr].playlistID, m_ImageManager.GetDefaultPlaylistImage());
						else m_ImageManager.AddTexture(playlists[curr].playlistID, m_ImageManager.GetTextureFromImage(Image(playlists[curr].playlistCoverBuffer)));

					}
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::PopStyleColor();
			if (!error.empty()) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImGui::Text(error.c_str());
				ImGui::PopStyleColor();
			}
			ImGui::EndPopup();
		}
	}

	void PlaylistModule::DeletePlaylistModal() {
		ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImVec2 modalSize = ImVec2(100.0f, 100.0f);
		ImVec2 modalPos = ImVec2(center.x - modalSize.x * 0.5f, center.y - modalSize.y * 0.5f);
		ImGui::SetNextWindowPos(modalPos);
		ImGui::SetNextWindowSize(modalSize);

		if (ImGui::BeginPopupModal("Delete playlist", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
			if (ImGui::Button("Yes")) {
				std::string url = "http://localhost:3000/playlists/" + std::to_string(playlists[curr].playlistID);
				std::string token;
				m_AuthManager.RetrieveToken("accessToken", token);
				m_NetworkManager.DeleteRequest(url, token);
				m_MessageManager.Publish("updateUser", "");
				playlists[curr] = Playlist();
				ImGui::CloseCurrentPopup();
				m_MessageManager.Publish("homeButton", std::string("FrontPage"));
			}
			ImGui::SameLine();
			if (ImGui::Button("No")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::PopStyleColor();
			ImGui::EndPopup();
		}
	}
}