#include "PlaylistListModule.h"

namespace Thingy {
	void PlaylistListModule::SetupSubscriptions() {
		m_MessageManager.Subscribe("userChanged", GetModuleName(), [this](const MessageData data) {
			UpdateUserInfo();
			});
		m_MessageManager.Subscribe("loggedIn", GetModuleName(), [this](const MessageData data) {
			if (data.type() == typeid(bool)) {
				loggedIn = std::any_cast<bool>(data);
			}
			});
	}

	void PlaylistListModule::OnLoad(const std::variant<int, std::string> moduleState) {}

	void PlaylistListModule::OnUpdate() {}

	void PlaylistListModule::Window() {
		ImVec2 barSize = ImVec2(GetSize().x - 20, 30);
		DragBar(upProps, barSize);

		if (ImGui::BeginTable("PlaylistTable", 2, ImGuiTableFlags_ScrollY, ImVec2(ImGui::GetWindowSize().x - 5, ImGui::GetWindowSize().y - 100.0f))) {
			ImGui::TableSetupColumn("Cover", ImGuiTableColumnFlags_WidthFixed, 80.0f);
			ImGui::TableSetupColumn("Info", ImGuiTableColumnFlags_WidthFixed, 200.0f);

			int i = 0;
			for (auto& playlist : user.playlists) {

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Image(m_ImageManager.GetPlaylistImTexture(playlist.playlistID), { 80.0f, 80.0f });
				ImGui::TableSetColumnIndex(1);
				std::string label = std::to_string(i);
				if (ImGui::Selectable(std::string("##" + label).c_str(), false, ImGuiSelectableFlags_AllowOverlap, ImVec2(0, 80.0f))) {
					m_MessageManager.Publish("openPlaylist", playlist);
					m_MessageManager.Publish("changeScene", std::string("PlaylistScene"));
				}

				ImGui::SameLine();
				ImGui::BeginGroup();
				ImGui::Text(U8(playlist.playlistName.c_str()));
				ImGui::EndGroup();
				i++;
			}
			for (auto& follow : user.followed) {
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Image((follow.second == PLAYLIST ? m_ImageManager.GetPlaylistImTexture(follow.first) : m_ImageManager.GetImTexture(follow.first)), { 80.0f, 80.0f });
				ImGui::TableSetColumnIndex(1);
				std::string label = std::to_string(i);
				if (ImGui::Selectable(std::string("##" + label).c_str(), false, ImGuiSelectableFlags_AllowOverlap, ImVec2(0, 80.0f))) {
					if (follow.second == PLAYLIST) {
						m_MessageManager.Publish("openPlaylist", followedPlaylists[follow.first]);
						m_MessageManager.Publish("changeScene", std::string("PlaylistScene"));
					} else if (follow.second == ALBUM) {
						m_MessageManager.Publish("openAlbum", follow.first);
						m_MessageManager.Publish("changeScene", std::string("AlbumScene"));
					} else if (follow.second == ARTIST) {
						m_MessageManager.Publish("openArtist", follow.first);
						m_MessageManager.Publish("changeScene", std::string("ArtistScene"));
					}
				}

				ImGui::SameLine();
				ImGui::BeginGroup();
				if (follow.second == PLAYLIST) {
					ImGui::Text(U8(followedPlaylists[follow.first].playlistName.c_str()));
				} else {
					ImGui::Text(U8(followedAlbumArtist[follow.first].c_str()));
				}
				ImGui::EndGroup();
				i++;
			}

			ImGui::EndTable();
		}
		if (ImGui::Button("Create New Playlist")) {
			error = "";
			newDescription = "";
			newPlaylistName = "";
			isPrivate = false;
			coverPath = "";
			newPlaylistCover = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetDefaultPlaylistImage());
			ImGui::OpenPopup("New playlist");
		}

		NewPlaylistModal();

	}

	uint16_t PlaylistListModule::OnRender() {
		upProps &= BIT(0);
		if (loggedIn) {
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
		}
		return upProps;
	}

	void PlaylistListModule::UpdateUserInfo() {
		std::unordered_map<uint32_t, std::future<Image>> images;

		for (auto& playlist : user.playlists) {
			if (playlist.playlistCoverBuffer.empty()) {
				m_ImageManager.AddPlaylistTexture(playlist.playlistID, m_ImageManager.GetDefaultPlaylistImage());
			} else {
				m_ImageManager.AddPlaylistTexture(playlist.playlistID, m_ImageManager.GetTextureFromImage(Image(playlist.playlistCoverBuffer)));
			}
		}
		for (auto& follow : user.followed) {
			std::cout << follow.second << "\n";
			if (follow.second == PLAYLIST) {
			
				std::string url = "http://localhost:3000/playlists/" + std::to_string(follow.first);
				std::string response = m_NetworkManager.GetRequest(url);
				try {
					Playlist newPlaylist = json::parse(response);
					followedPlaylists[follow.first] = newPlaylist;
					if (newPlaylist.playlistCoverBuffer.empty()) {
						m_ImageManager.AddPlaylistTexture(follow.first, m_ImageManager.GetDefaultPlaylistImage());
					} else {
						m_ImageManager.AddPlaylistTexture(follow.first, m_ImageManager.GetTextureFromImage(Image(newPlaylist.playlistCoverBuffer)));
					}
				} catch (const nlohmann::json::parse_error& e) {
						T_ERROR("JSON parse error: {0}", e.what());
				}
				
			} else if (follow.second == ALBUM) {
				std::string url = "https://api.jamendo.com/v3.0/tracks/?client_id=" + std::string(CLIENTID) + "&format=jsonpretty&album_id=" + std::to_string(follow.first);
				T_INFO("{0}", url);
				Track track = m_NetworkManager.GetTrack(url)[0];
				followedAlbumArtist[follow.first] = track.albumName;
				if (!m_ImageManager.HasTextureAt(follow.first)) {
					
					m_ImageManager.AddTexture(follow.first, m_ImageManager.GetTexture(track.imageURL));
				}
			} else if (follow.second == ARTIST) {
				std::string url = "https://api.jamendo.com/v3.0/artists/?client_id=" + std::string(CLIENTID) + "&format=jsonpretty&id=" + std::to_string(follow.first);
				T_INFO("{0}", url);
				Artist artist = m_NetworkManager.GetArtist(url)[0];
				followedAlbumArtist[follow.first] = artist.artistName;
				if (!m_ImageManager.HasTextureAt(follow.first)) {
					if (artist.artistImageURL == "") {
						m_ImageManager.AddTexture(follow.first, m_ImageManager.GetDefaultArtistImage());
					} else {
						m_ImageManager.AddTexture(follow.first, m_ImageManager.GetTexture(artist.artistImageURL));
					}
				}
			}
		}
	}

	void PlaylistListModule::NewPlaylistModal() {
		ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImVec2 modalSize = ImVec2(310.0f, 600.0f);
		ImVec2 modalPos = ImVec2(center.x - modalSize.x * 0.5f, center.y - modalSize.y * 0.5f);
		ImGui::SetNextWindowPos(modalPos);
		ImGui::SetNextWindowSize(modalSize);

		if (ImGui::BeginPopupModal("New playlist", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
			ImGui::SameLine(ImGui::GetWindowWidth() - 20);
			if (ImGui::Button("X")) ImGui::CloseCurrentPopup();

			ImGui::Image((ImTextureID)(intptr_t)newPlaylistCover.get(), ImVec2(100.0f, 100.0f));
			if (ImGui::IsItemHovered()) {
				upProps |= BIT(3);
			}
			if (ImGui::IsItemClicked()) {
				std::string imagePath;
				if (!OpenFileExplorer(imagePath)) {
					T_ERROR("OpenFileExplorer failed!");
				} else {
					if (IsImageFile(imagePath)) {
						coverPath = imagePath;
						newPlaylistCover = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTextureFromFile(coverPath.c_str()));
						
					} else {
						T_INFO("not image path: {0}", imagePath);
					}
				}

			}
			ImGui::InputText("Playlist Name", &newPlaylistName, 0, ResizeCallback, (void*)&newPlaylistName);
			ImGui::InputText("Description", &newDescription, 0, ResizeCallback, (void*)&newDescription);
			ImGui::Checkbox("Private?", &isPrivate);
			if (ImGui::Button("Save Playlist")) {
				if (newPlaylistName.size() < 3) {
					error = "Playlist name cannot be shorter than 3 characters!";
				} else {
					std::string url = "http://localhost:3000/playlists";
					std::string token;
					m_AuthManager.RetrieveToken("accessToken", token);
					m_NetworkManager.AddPlaylist(url, newPlaylistName, newDescription, isPrivate, coverPath, token);
					m_MessageManager.Publish("updateUser", "");
					ImGui::CloseCurrentPopup();
				}
			}
			if (!error.empty()) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImGui::Text(error.c_str());
				ImGui::PopStyleColor();
			}
			ImGui::EndPopup();
		}
	}
}