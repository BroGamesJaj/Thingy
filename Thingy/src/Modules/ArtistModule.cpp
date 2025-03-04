#include "ArtistModule.h"


namespace Thingy {

	void ArtistModule::SetupSubscriptions() {
		m_MessageManager->Subscribe("openArtist", GetModuleName(), [this](const MessageData data) {

			if (data.type() == typeid(Artist)) {
				Artist recArtist = std::any_cast<Artist>(data);
				for (size_t i = 0; i < artists.size(); i++) {
					if (artists[i].artistName == recArtist.artistName) {
						curr = i;
						T_INFO("returned");
						return;
					}
				}
				
				std::string url = 
					"https://api.jamendo.com/v3.0/artists/tracks/?client_id=" 
					+ std::string(CLIENTID) 
					+ "&format=jsonpretty&limit=1&id=" 
					+ std::to_string(recArtist.id);

				std::vector<Artist> getArtist = m_NetworkManager->GetArtistsWithTracks(url);
				if (getArtist.size() < 1) {
					T_ERROR("ArtistModule: GetArtistsWithTracks returned 0 artists.");
					return;
				}
				artists.emplace_back(getArtist[0]);
				T_INFO("-1curr: {0}", curr);
				curr = artists.size() - 1;
				std::cout << artists[curr].toString() << std::endl;
				T_INFO("0curr: {0}", curr);
			} else {
				T_ERROR("ArtistModule: Invalid data type for openArtist");
			}
			});

		m_MessageManager->Subscribe("beforeSwitch" + GetModuleName(), GetModuleName(), [this](const MessageData data) {
			m_MessageManager->Publish("saveModuleState", std::make_pair<std::string, std::variant<int, std::string>>(GetModuleName(), artists[curr].artistName));
			});

	}

	void ArtistModule::OnLoad(const std::variant<int, std::string> moduleState) {
		T_INFO("1curr: {0}", curr);
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
		int i = 0;
		std::unordered_map<uint32_t, std::future<Image>> images;
		for (auto& album : artists[curr].albums) {
			if (!albumTextures[artists[curr].id][album.id]) {
				//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
				std::string& url = album.imageURL;
				images.emplace(album.id, std::async(std::launch::async, [this, &url]() { return m_ImageManager->GetImage(url); }));
				//albumTextures[artists[curr].id][album.id] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager->GetTexture(album.imageURL));
				//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
				//std::cout << "full time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[milliseconds]" << std::endl;
			}
			
		}
		T_INFO("fetches started");
		while (!images.empty()) {
			for (auto it = images.begin(); it != images.end(); ) {
				auto& image = it->second;
				if (image.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
					albumTextures[artists[curr].id][it->first] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager->GetTextureFromImage(image.get()));
					it = images.erase(it);
				} else {
					T_TRACE("not done");
					++it;
				}
			}
		}
		T_INFO("images done");

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
		for (auto& album : artists[curr].albums) {
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