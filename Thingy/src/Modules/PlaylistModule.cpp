#include "PlaylistModule.h"


namespace Thingy {

	void PlaylistModule::SetupSubscriptions() {
		m_MessageManager.Subscribe("openPlaylist", GetModuleName(), [this](const MessageData data) {

			if (data.type() == typeid(Playlist)) {
				Playlist playlist = std::any_cast<Playlist>(data);
				for (size_t i = 0; i < playlists.size(); i++) {
					if (playlists[i].playlistName == playlist.playlistName) {
						curr = i;
						T_INFO("returned");
						return;
					}
				}
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
				playlists.push_back(playlist);
				curr = playlists.size() - 1;
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

		if (playlists[curr].playlistCoverBuffer.empty())
			playlistCover = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetDefaultPlaylistImage());
		else playlistCover = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTextureFromImage(Image(playlists[curr].playlistCoverBuffer)));
		std::unordered_map<uint32_t, std::future<Image>> images;
		length = 0;
		for (auto& trackId : playlists[curr].trackIDs) {
			if (!textures[trackId]) {
				std::string& url = tracks[trackId].imageURL;
				images.emplace(trackId, std::async(std::launch::async, [this, &url]() { return m_ImageManager.GetImage(url); }));
			}
			length += tracks[trackId].duration;
		}
		while (!images.empty()) {
			for (auto it = images.begin(); it != images.end(); ) {
				auto& image = it->second;
				if (image.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
					textures[it->first] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTextureFromImage(image.get()));
					it = images.erase(it);
				} else {
					++it;
				}
			}
		}

	}

	void PlaylistModule::OnUpdate() {}

	void PlaylistModule::Window() {
		ImVec2 barSize = ImVec2(GetSize().x - 20, 30);
		DragBar(upProps, barSize);

		ImGui::Image(reinterpret_cast<ImTextureID>(playlistCover.get()), { 300.0f, 300.0f });
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::Text(U8(playlists[curr].playlistName.c_str()));
		ImGui::Text(U8(playlists[curr].description.c_str()));
		ImGui::Text("Track count: %zu", playlists[curr].trackIDs.size());
		ImGui::SameLine();
		ImGui::Text("Playlist length: %s", SecondsToTimeString(length).c_str());
		ImGui::EndGroup();
		ImGui::BeginChild("Tracks", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);
		
		for (size_t i = 0; i < playlists[curr].trackIDs.size(); i++) {
			int& trackId = playlists[curr].trackIDs[i];
			ImGui::BeginGroup();
			ImGui::Image(reinterpret_cast<ImTextureID>(textures[trackId].get()), { 200.0f, 200.0f });
			if (ImGui::IsItemClicked()) {
				std::vector<Track> queueTracks;
				for (size_t j = i; j < playlists[curr].trackIDs.size(); j++) {
					queueTracks.push_back(tracks[playlists[curr].trackIDs[j]]);
				}
				std::vector<Track> history;
				for (size_t j = 0; j < i; j++) {
					history.push_back(tracks[playlists[curr].trackIDs[j]]);
				}
				m_MessageManager.Publish("newHistory", history);
				m_AudioManager.ClearQueue();
				m_MessageManager.Publish("addToQueue", queueTracks);
				m_MessageManager.Publish("startMusic", "");
			};
			LimitedTextWrap(tracks[trackId].title.c_str(), 180, 3);
			ImGui::EndGroup();
			ImGui::SameLine();
		}
		
		ImGui::EndChild();

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
}