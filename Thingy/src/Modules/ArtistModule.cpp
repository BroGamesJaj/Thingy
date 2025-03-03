#include "ArtistModule.h"


namespace Thingy {

	void ArtistModule::SetupSubscriptions() {
		m_MessageManager->Subscribe("openArtist", GetModuleName(), [this](const MessageData data) {

			if (data.type() == typeid(Artist)) {
				Artist recAlbum = std::any_cast<Artist>(data);
				for (size_t i = 0; i < artists.size(); i++) {
					if (artists[i].artistName == recAlbum.artistName) {
						curr = i;
						T_INFO("returned");
						return;
					}
				}
				artists.emplace_back(recAlbum);
				curr = artists.size() - 1;
				if (artists[curr].albums.empty()) {
					artists[curr].albums = m_NetworkManager->GetAlbum("https://api.jamendo.com/v3.0/albums/tracks/?client_id=" + std::string(CLIENTID) + "&format=jsonpretty&limit=200&artist_id=" + std::to_string(artists[curr].id));
				}
				std::cout << artists[curr].toString() << std::endl;
			} else {
				T_ERROR("ArtistModule: Invalid data type for openArtist");
			}
			});

		m_MessageManager->Subscribe("beforeSwitch" + GetModuleName(), GetModuleName(), [this](const MessageData data) {
			m_MessageManager->Publish("saveModuleState", std::make_pair<std::string, std::variant<int, std::string>>(GetModuleName(), artists[curr].artistName));
			});

	}

	void ArtistModule::OnLoad(const std::variant<int, std::string> moduleState) {
		if (std::holds_alternative<std::string>(moduleState)) {
			std::string artistName = std::get<std::string>(moduleState);
			for (size_t i = 0; i < artists.size(); i++) {
				if (artists[i].artistName == artistName) {
					curr = i;
				}
			}
		}
		if (std::holds_alternative<int>(moduleState)) {
			T_ERROR("artist got: {0}", std::get<int>(moduleState));
		}
		T_INFO("curr: {0}", curr);
		if (!textures[artists[curr].id]) {
			if (artists[curr].artistImageURL == "") {
			textures[artists[curr].id] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager->GetDefaultArtistImage());
			} else {
				textures[artists[curr].id] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager->GetTexture(artists[curr].artistImageURL));
			}

		}
		for (auto& album : albums[artists[curr].id]) {
			if (!albumTextures[artists[curr].id][album.id]) {
				albumTextures[artists[curr].id][album.id] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager->GetTexture(album.imageURL));
			}
		}

	}

	void ArtistModule::OnUpdate() {}

	void ArtistModule::Window() {

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

		ImGui::Image((ImTextureID)(intptr_t)textures[artists[curr].id].get(), { 300.0f, 300.0f });
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::Text(artists[curr].artistName.data());
		ImGui::Text("Track count: %zu", artists[curr].albums.size());
		ImGui::SameLine();
		ImGui::Text("length");
		ImGui::EndGroup();
		ImGui::BeginChild("Albums", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);
		for (auto& album : albums[artists[curr].id]) {
			ImGui::BeginGroup();
			ImGui::Image((ImTextureID)(intptr_t)albumTextures[artists[curr].id][album.id].get(), { 200.0f, 200.0f });
			LimitedTextWrap(album.name.data(), 180, 3);
			ImGui::EndGroup();
			ImGui::SameLine();
		}
		ImGui::EndChild();

	}

	uint16_t ArtistModule::OnRender() {
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