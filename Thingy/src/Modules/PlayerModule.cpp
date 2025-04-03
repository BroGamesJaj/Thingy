#include "tpch.h"
#include "PlayerModule.h"
namespace Thingy {

	void PlayerModule::SetupSubscriptions() {
		m_MessageManager.Subscribe("openPlayer", GetModuleName(), [this](MessageData data) {
			open = true;
			});

		m_MessageManager.Subscribe("closePlayer", GetModuleName(), [this](MessageData data) {
			open = false;
			});

		m_MessageManager.Subscribe("changeQueueOpen", GetModuleName(), [this](MessageData data) {
			isQueueOpen = !isQueueOpen;
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

	void PlayerModule::OnLoad(const std::variant<int, std::string> moduleState) {}

	void PlayerModule::OnUpdate() {
		int currentTrackID = m_AudioManager.GetCurrentTrack().id;
		if (isQueueOpen) {

			std::unordered_map<uint32_t, std::future<Image>> images;
			for (auto& track : queue) {
				if (!m_ImageManager.HasTextureAt(track.albumID)) {
					std::string& url = track.imageURL;
					images.emplace(track.albumID, std::async(std::launch::async, [this, &url]() { return m_ImageManager.GetImage(url); }));
				}

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
		}

		if (currentTrackID != currentTrack.id && currentTrackID != -1) {
			open = true;
			currentTrack = m_AudioManager.GetCurrentTrack();
			if (!m_ImageManager.HasTextureAt(currentTrack.albumID)) {
				m_ImageManager.AddTexture(currentTrack.albumID, m_ImageManager.GetTexture(currentTrack.imageURL));
			}
		}
	}

	void PlayerModule::Window() {
		ImVec2 barSize = ImVec2(GetSize().x - 20, 30);
		DragBar(upProps, barSize);
		if (isQueueOpen) {
			QueueView();
		} else {
			PlayerView();
		}
		if (addTrackToPlaylist) {
			ImGui::OpenPopup("Add to playlists");
			addTrackToPlaylist = false;
		}
		PlaylistModal();

	}

	uint16_t PlayerModule::OnRender() {
		upProps &= BIT(0);
		if (m_AudioManager.GetQueue().size() != 0 && open) {
			ImGui::Begin(GetModuleName().c_str(), nullptr, defaultWindowFlags);
			Window();
			ImGui::End();
			if (upProps & BIT(0)) {
				ImGui::BeginDisabled();
				ImGui::Begin("floater", nullptr, defaultWindowFlags);
				Window();
				ImGui::SetWindowPos({ ImGui::GetMousePos().x - (ImGui::FindWindowByName(GetModuleName().c_str())->Size.x / 2), ImGui::GetMousePos().y + 5 });
				ImGui::SetWindowSize(ImGui::FindWindowByName(GetModuleName().c_str())->Size);
				ImGui::End();
				ImGui::EndDisabled();
			}
		}
		return upProps;
	}

	void PlayerModule::QueueView() {
		if (ImGui::BeginTable("QueueTable", 2, ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY, ImVec2(0, ImGui::GetWindowSize().y - 100.0f))) {
			ImGui::TableSetupColumn("Cover", ImGuiTableColumnFlags_WidthFixed, 80.0f);
			ImGui::TableSetupColumn("Info");

			int i = 0;
			for (auto& track : queue) {
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Image(m_ImageManager.GetImTexture(track.albumID), { 80.0f, 80.0f });

				ImGui::TableSetColumnIndex(1);
				std::string label = std::to_string(i);
				ImGui::Selectable(std::string("##" + label).c_str(), false, ImGuiSelectableFlags_AllowOverlap, ImVec2(0, 80.0f));
				if (ImGui::IsItemClicked()) m_AudioManager.ChangeMusicByTrack(track.id);
				if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
					lastClickedIndex = i;
					lastClickedTrack = track;
					ImGui::OpenPopup("QueueOptions");
				}



				ImGui::SameLine();
				ImGui::BeginGroup();
				ImGui::Text(U8(track.title.c_str()));
				ImGui::Text(U8(track.artistName.c_str()));
				ImGui::EndGroup();

				i++;
			}
			if (ImGui::BeginPopup("QueueOptions")) {
				if (ImGui::Button("Remove from Queue")){
					m_AudioManager.RemoveFromQueue(lastClickedIndex);
					ImGui::CloseCurrentPopup();
				}
				if (loggedIn) {
					if (ImGui::Button("Add to Playlists")) {
						selectedPlaylists.clear();
						for (auto& playlist : user.playlists) {
							if (std::find(playlist.trackIDs.begin(), playlist.trackIDs.end(), lastClickedTrack.id) != playlist.trackIDs.end()) {
								selectedPlaylists[playlist.playlistID] = true;
							} else {
								selectedPlaylists[playlist.playlistID] = false;
							}
						}
						addTrackToPlaylist = true;
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndPopup();
			}
			ImGui::EndTable();
		}
		ImVec2 wSize = ImGui::GetWindowSize();
		ImGui::SetCursorPos(ImVec2(wSize.x / 2 - 50, wSize.y - 50));
		if (ImGui::Button("Player")) {
			isQueueOpen = !isQueueOpen;
		}
	}

	void PlayerModule::PlayerView() {
		if (m_AudioManager.GetQueue().size() == 0) {
			ImGui::Button("image", { 300.0f, 300.0f });
		} else {
			ImGui::Image(m_ImageManager.GetImTexture(currentTrack.albumID), {300.0f, 300.0f});
		}
		ImGui::Text(U8(currentTrack.title.c_str()));
		ImGui::Text(U8(currentTrack.artistName.c_str()));
		if (ImGui::IsItemHovered()) {
			upProps |= BIT(3);
		}
		if (ImGui::IsItemClicked()) {
			m_MessageManager.Publish("openArtist", currentTrack.artistID);
			m_MessageManager.Publish("changeScene", std::string("ArtistScene"));
		}

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushFont(Fonts::icons20);
		if (loggedIn) {
			if (ImGui::Button(ICON_FA_HEART, ImVec2(30.0f, 30.0f))) {
				if (std::find(user.playlists[0].trackIDs.begin(), user.playlists[0].trackIDs.end(), currentTrack.id) == user.playlists[0].trackIDs.end()) {
					std::string url = "http://localhost:3000/playlists/add?playlistIds=" + std::to_string(user.playlists[0].playlistID);
					url += "&trackId=" + std::to_string(currentTrack.id);
					std::string token;
					m_AuthManager.RetrieveToken("accessToken", token);
					std::string json;
					m_NetworkManager.PostRequestAuth(url, json, token);
					m_MessageManager.Publish("updateUser", "");
				}
			
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_BACKWARD, { 30.0f, 30.0f })) {
			m_AudioManager.PrevTrack();
		}
		ImGui::SameLine();
		if (m_AudioManager.IsMusicPaused()) {
			if (ImGui::Button(ICON_FA_PLAY, { 30.0f, 30.0f })) {
				m_AudioManager.ResumeMusic();
			}
		} else {
			if (ImGui::Button(ICON_FA_PAUSE, { 30.0f, 30.0f })) {
				m_AudioManager.PauseMusic();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_FORWARD, { 30.0f, 30.0f })) {
			m_AudioManager.NextTrack();
		};
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_SHUFFLE, { 30.0f, 30.0f })) {
			m_AudioManager.ShuffleQueue();
		};
		if (loggedIn) {
			if (ImGui::Button("Add to playlists")) {
				selectedPlaylists.clear();
				for (auto& playlist : user.playlists) {
					if (std::find(playlist.trackIDs.begin(), playlist.trackIDs.end(), currentTrack.id) != playlist.trackIDs.end()) {
						selectedPlaylists[playlist.playlistID] = true;
					} else {
						selectedPlaylists[playlist.playlistID] = false;
					}
				}
				lastClickedTrack = currentTrack;
				ImGui::OpenPopup("Add to playlists");
			}
		}
		ImGui::PopFont();
		ImGui::PopStyleColor();
		ImGui::Text("Current");
		ImGui::SameLine();
		ImGui::SliderInt("time", &m_AudioManager.GetCurrentTrackPos(), 0, m_AudioManager.GetCurrentTrackDuration());
		if (ImGui::IsItemEdited()) {
			m_AudioManager.ChangeMusicPos();
		}
		ImGui::SliderInt("Volume", &m_AudioVolume, 0, MIX_MAX_VOLUME);
		if (ImGui::IsItemEdited()) {
			m_AudioManager.ChangeVolume();
		}
		ImVec2 wSize = ImGui::GetWindowSize();
		ImGui::SetCursorPos(ImVec2(wSize.x / 2 - 50, wSize.y - 50));
		if (ImGui::Button("Queue")) {
			isQueueOpen = !isQueueOpen;
		}

	}

	void PlayerModule::PlaylistModal() {
		
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
					if (!selectedPlaylists[playlist.playlistID])
						selectedPlaylists[playlist.playlistID] = false;
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Image(m_ImageManager.GetImTexture(playlist.playlistID), { 80.0f, 80.0f });
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
						if (selectedPlaylists[playlist.playlistID] && std::find(playlist.trackIDs.begin(), playlist.trackIDs.end(), lastClickedTrack.id) == playlist.trackIDs.end()) {
							hasSelected = true;
							url += std::to_string(playlist.playlistID) + ",";
						}
					}
					if (hasSelected) {
						url.pop_back();
					}
					url += "&trackId=" + std::to_string(lastClickedTrack.id);
					if (hasSelected) {
						std::string token;
						m_AuthManager.RetrieveToken("accessToken", token);
						std::string json;
						m_NetworkManager.PostRequestAuth(url, json, token);
						m_MessageManager.Publish("updateUser", "");
					}
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}

	void PlayerModule::UserInfoChanged() {
		for (size_t i = 0; i < user.playlists.size(); i++) {
			const Playlist& currP = user.playlists[i];
			if (currP.playlistCoverBuffer.empty()) {
				m_ImageManager.AddTexture(currP.playlistID, m_ImageManager.GetDefaultPlaylistImage());
			} else {
				m_ImageManager.AddTexture(currP.playlistID, m_ImageManager.GetTextureFromImage(Image(currP.playlistCoverBuffer)));
			};
		}
	}
}