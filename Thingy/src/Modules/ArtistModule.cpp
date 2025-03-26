#include "ArtistModule.h"


namespace Thingy {

	void ArtistModule::SetupSubscriptions() {
		m_MessageManager.Subscribe("openArtist", GetModuleName(), [this](const MessageData data) {

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

				std::vector<Artist> getArtist = m_NetworkManager.GetArtistsWithTracks(url);
				if (getArtist.size() < 1) {
					T_ERROR("ArtistModule: GetArtistsWithTracks returned 0 artists.");
					return;
				}
				artists.emplace_back(getArtist[0]);
				
				curr = artists.size() - 1;
				T_INFO("{0}", artists[curr].toString());
			} else if(data.type() == typeid(int)){
				int artistID = std::any_cast<int>(data);
				std::string url =
					"https://api.jamendo.com/v3.0/artists/tracks/?client_id="
					+ std::string(CLIENTID)
					+ "&format=jsonpretty&limit=1&id="
					+ std::to_string(artistID);

				std::vector<Artist> getArtist = m_NetworkManager.GetArtistsWithTracks(url);
				if (getArtist.size() < 1) {
					T_ERROR("ArtistModule: GetArtistsWithTracks returned 0 artists.");
					return;
				}
				artists.emplace_back(getArtist[0]);

				curr = artists.size() - 1;
				T_INFO("{0}", artists[curr].toString());
			} else {
				T_ERROR("ArtistModule: Invalid data type for openArtist");
			}
			});

		m_MessageManager.Subscribe("beforeSwitch" + GetModuleName(), GetModuleName(), [this](const MessageData data) {
			m_MessageManager.Publish("saveModuleState", std::make_pair<std::string, std::variant<int, std::string>>(GetModuleName(), artists[curr].artistName));
			});

		m_MessageManager.Subscribe("loggedIn", GetModuleName(), [this](const MessageData data) {
			if (data.type() == typeid(bool)) {
				loggedIn = std::any_cast<bool>(data);
			}
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
		if (!m_ImageManager.HasTextureAt(artists[curr].id)) {
			if (artists[curr].artistImageURL == "") {
				m_ImageManager.AddTexture(artists[curr].id, m_ImageManager.GetDefaultArtistImage());
			} else {
				m_ImageManager.AddTexture(artists[curr].id, m_ImageManager.GetTexture(artists[curr].artistImageURL));
			}

		}
		imageFutures.clear();
		
		for (auto& album : artists[curr].albums) {
			if (!m_ImageManager.HasTextureAt(album.id)) {
				std::string& url = album.imageURL;
				imageFutures.emplace(album.id, std::async(std::launch::async, [this, &url]() { return m_ImageManager.GetImage(url); }));
			}
			
		}
	}

	void ArtistModule::OnUpdate() {
		if (!imageFutures.empty()) {
			for (auto it = imageFutures.begin(); it != imageFutures.end(); ) {
				auto& image = it->second;
				if (image.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
					m_ImageManager.AddTexture(it->first, m_ImageManager.GetTextureFromImage(image.get()));
					it = imageFutures.erase(it);
				} else {
					++it;
				}
			}
		}
	}

	void ArtistModule::Window() {
		ImVec2 barSize = ImVec2(GetSize().x - 20, 30);
		DragBar(upProps, barSize);

		ImGui::Image(m_ImageManager.GetImTexture(artists[curr].id), { 300.0f, 300.0f });
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::Text(U8(artists[curr].artistName.c_str()));
		ImGui::Text("Album count: %zu", artists[curr].albums.size());
		ImGui::SameLine();
		ImGui::Text("length");
		ImGui::EndGroup();
		ImGui::BeginChild("Albums", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);
		for (size_t i = 0; i < artists[curr].albums.size(); i++) {
			Album& album = artists[curr].albums[i];
			ImGui::BeginGroup();
			ImGui::Image(m_ImageManager.GetImTexture(album.id), {200.0f, 200.0f});
			if (ImGui::IsItemClicked()) {
				m_MessageManager.Publish("openAlbum", album);
				m_MessageManager.Publish("changeScene", std::string("AlbumScene"));

			}
			LimitedTextWrap(album.name.c_str(), 180, 3);
			ImGui::EndGroup();
			ImGui::SameLine();
		}
		ImGui::EndChild();

	}

	uint16_t ArtistModule::OnRender() {
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