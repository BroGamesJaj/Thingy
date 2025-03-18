#include "SearchModule.h"

namespace Thingy {
	void SearchModule::SetupSubscriptions() {
		m_MessageManager.Subscribe("openSearch", GetModuleName(), [this](MessageData data) {
			if (data.type() == typeid(std::pair<std::string, int>)) {
				std::pair<std::string, int> pair = std::any_cast<std::pair<std::string, int>>(data);
				std::string searchTerm = pair.first;
				whichToggled = pair.second;
				trackResults.clear();
				switch (whichToggled) {
				case 0: { //all

					T_INFO("0");
					break;
				}
				case 1: { //tags

					std::string url = "https://api.jamendo.com/v3.0/tracks/?client_id=" + std::string(CLIENTID) + "&format=jsonpretty&limit=50&tags=" + searchTerm;
					std::string response = m_NetworkManager.GetRequest(url);

					break;
				}
				case 2: { //tracks
					std::string url = "https://api.jamendo.com/v3.0/tracks/?client_id=" + std::string(CLIENTID) + "&format=jsonpretty&limit=50&search=" + searchTerm;
					T_INFO("url: {0}", url);
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
				case 3: { //albums

					T_INFO("3");
					break;
				}
				case 4: { //artists

					T_INFO("4");
					break;
				}
				case 5: { //playlists

					T_INFO("5");
					break;
				}
				default:
					break;
				}
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

	void SearchModule::OnLoad(const std::variant<int, std::string> moduleState) {
		std::unordered_map<uint32_t, std::future<Image>> images;	
		while (!images.empty()) {
			for (auto it = images.begin(); it != images.end(); ) {
				auto& image = it->second;
				if (image.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
					textures[0101] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTextureFromImage(image.get()));
					it = images.erase(it);
				} else {
					++it;
				}
			}
		}
	}

	void SearchModule::OnUpdate() {}

	void SearchModule::Window() {
		//TODO: Show search results.
		switch (whichToggled) {
		case 0: { //all

			T_INFO("0");
			break;
		}
		case 1: { //tags
			break;
		}
		case 2: { //tracks
			TrackResultsDisplay();
			break;
		}
		case 3: { //albums

			T_INFO("3");
			break;
		}
		case 4: { //artists

			T_INFO("4");
			break;
		}
		case 5: { //playlists

			T_INFO("5");
			break;
		}
		default:
			break;
		}
	}

	uint16_t SearchModule::OnRender() {
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

	void SearchModule::TrackResultsDisplay() {
		ImGui::BeginTable("trackDisplay",2);
		for (size_t i = 0; i < trackResults.size(); i++) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Button(std::to_string(i).data(), ImVec2(100.0f, 100.0f));
			ImGui::TableSetColumnIndex(1);
			ImGui::Text(U8(trackResults[i].title.c_str()));
			ImGui::Text(U8(trackResults[i].albumName.c_str()));
			ImGui::Text(U8(trackResults[i].artistName.c_str()));
		}
		ImGui::EndTable();
	}
}