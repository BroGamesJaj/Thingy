#include "PlaylistModule.h"


namespace Thingy {

	void PlaylistModule::SetupSubscriptions() {
		m_MessageManager.Subscribe("openPlaylist", GetModuleName(), [this](const MessageData data) {

			if (data.type() == typeid(Playlist)) {
				//TODO opening
			} else {
				T_ERROR("PlaylistModule: Invalid data type for openAlbum");
			}
			});

		m_MessageManager.Subscribe("beforeSwitch" + GetModuleName(), GetModuleName(), [this](const MessageData data) {
			m_MessageManager.Publish("saveModuleState", std::make_pair<std::string, std::variant<int, std::string>>(GetModuleName(), playlists[curr].playlistName));
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
			T_ERROR("album got: {0}", std::get<int>(moduleState));
		}

		//TODO: texture loading
	}

	void PlaylistModule::OnUpdate() {}

	void PlaylistModule::Window() {

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

		ImGui::Image(reinterpret_cast<ImTextureID>(playlistCover.get()), { 300.0f, 300.0f });
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::Text(playlists[curr].playlistName.data());
		ImGui::Text(playlists[curr].description.data());
		ImGui::Text("Track count: %zu", playlists[curr].trackIDs.size());
		ImGui::SameLine();
		//TODO: playlist length
		ImGui::Text("Playlist length: %s", SecondsToTimeString(length).data());
		ImGui::EndGroup();
		ImGui::BeginChild("Tracks", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);

		// TODO show tracks
		/*
		for (size_t i = 0; i < album[curr].tracks.size(); i++) {
			Track& track = album[curr].tracks[i];
			ImGui::BeginGroup();
			ImGui::Image(reinterpret_cast<ImTextureID>(textures[album[curr].id].get()), { 200.0f, 200.0f });
			if (ImGui::IsItemClicked()) {
				std::vector<Track> tracks(album[curr].tracks.begin() + i, album[curr].tracks.end());
				m_AudioManager.ClearQueue();
				m_MessageManager.Publish("addToQueue", tracks);
				m_MessageManager.Publish("startMusic", "");
			};
			LimitedTextWrap(track.title.data(), 180, 3);
			ImGui::EndGroup();
			ImGui::SameLine();
		}
		*/
		ImGui::EndChild();

	}

	uint16_t PlaylistModule::OnRender() {
		ImGui::Begin(GetModuleName().data(), nullptr, defaultWindowFlags);
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