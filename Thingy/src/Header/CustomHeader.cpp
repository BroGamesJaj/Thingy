#include "tpch.h"
#include "CustomHeader.h"

namespace Thingy {

	CustomHeader::CustomHeader(MessageManager& messageManager, NetworkManager& networkManager, ImageManager& imageManager, AuthManager& authManager, SDL_Window* window, std::string& searchField) : m_MessageManager(messageManager), m_NetworkManager(networkManager), m_ImageManager(imageManager), m_AuthManager(authManager), user(authManager.GetUser()), m_Window(window), search(searchField) {
		m_MessageManager.Subscribe("loggedIn", "customHeader", [this](const MessageData data) {
			if (data.type() == typeid(bool)) {
				loggedIn = std::any_cast<bool>(data);
				if (loggedIn) {
					if (user.pfpBuffer.empty()) {
						pfpTexture = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetDefaultArtistImage());
					} else {
						pfpTexture = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTextureFromImage(Image(user.pfpBuffer)));
					}
				} else {
					pfpTexture = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetDefaultArtistImage());
				}

			}
			});

		m_MessageManager.Subscribe("userChanged", "customHeader", [this](const MessageData data) {
			if (loggedIn) {
				if (user.pfpBuffer.empty()) {
					pfpTexture = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetDefaultArtistImage());
				} else {
					pfpTexture = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTextureFromImage(Image(user.pfpBuffer)));
				}
			} else {
				pfpTexture = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetDefaultArtistImage());
			}
			});
	}

	void CustomHeader::OnRender() {
		int w, h;
		SDL_GetWindowSizeInPixels(m_Window, &w, &h);
		ImGui::SetNextWindowSize({ static_cast<float>(w), static_cast<float>(h) });
		ImGui::SetNextWindowPos({ 0.0f, 0.0f });
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f,0.0f,0.0f,1.0f));
		ImGui::Begin("Custom Header", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

		const float winW = ImGui::GetWindowWidth();
		const float scale = std::clamp(winW / 1280, 1.0f, 2.0f);
		ImGui::GetCurrentWindow()->DC.LayoutType = ImGuiLayoutType_Horizontal;
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushFont(Fonts::icons30);
		if (ImGui::Button(ICON_FA_HOUSE, { 38.0f, 38.0f })) {
			m_MessageManager.Publish("homeButton", std::string("FrontPage"));
		}
		if (ImGui::Button(ICON_FA_ARROW_LEFT, {38.0f, 38.0f})) {

			T_INFO("back");
			m_MessageManager.Publish("previousScene", "");
		}
		if (ImGui::Button(ICON_FA_ARROW_RIGHT, { 38.0f, 38.0f })) {
			T_INFO("next");
			m_MessageManager.Publish("nextScene", "");
		}
		ImGui::PopStyleColor();
		ImGui::PopFont();
		ImGui::SetCursorPosX(winW / 2 - std::clamp(200.0f * scale, 200.0f, 400.0f));
		ImGui::SetNextItemWidth(400.0f * scale);
		ImGui::PushFont(Fonts::size30);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
		ImGui::InputText("##search", &search, 0, ResizeCallback, static_cast<void*>(&search));
		if (ImGui::IsItemFocused()) {
			autoCompleteOn = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleVar();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		if (loggedIn) {
			CircleImage(reinterpret_cast<ImTextureID>(pfpTexture.get()), 38.0f);
			if (ImGui::IsItemClicked()) {
				ImGui::OpenPopup("profile");
			};
			
			if (ImGui::BeginPopup("profile", ImGuiWindowFlags_NoMove)) {
				if (ImGui::Button("Details")) {
					m_MessageManager.Publish("changeScene", std::string("ProfileScene"));
					ImGui::CloseCurrentPopup();
				};
				if (ImGui::Button("Logout")) {
					T_INFO("Logout");
					m_MessageManager.Publish("logout", "");
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		} else {
			if (ImGui::Button("Login", ImVec2(50.0f, 38.0f))) {
				m_MessageManager.Publish("changeScene", std::string("LoginScene"));
			}
		}
		ImGui::PushFont(Fonts::icons20);
		ImGui::SetCursorPosX(winW - 15 - 40 - 5 - 40 - 5 - 40);
		if (ImGui::Button(ICON_FA_WINDOW_MINIMIZE, { 38.0f, 38.0f }))
			m_MessageManager.Publish("minimize", 0);

		//ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 10 - 40 - 5 - 40); // 10 padding right, width of X button, padding between buttons, width of button
		if (ImGui::Button(ICON_FA_EXPAND, {38.0f, 38.0f})) {
			m_MessageManager.Publish("changeFullscreen", 0);
		}
		//ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 10 - 40); // 10 padding right, width of button
		if (ImGui::Button(ICON_FA_XMARK, { 38.0f, 38.0f }))
			m_MessageManager.Publish("closeWindow", 0);
		ImGui::PopFont();
		ImGui::PopStyleColor();
		ImGui::GetCurrentWindow()->DC.LayoutType = ImGuiLayoutType_Vertical;
		ImGui::End();
		ImGui::PopStyleColor();
		AutoComplete();
	}

	void CustomHeader::AutoComplete() {
		const ImVec4 onColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		const ImVec4 offColor = ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
		if (autoCompleteOn) {
			int w;
			SDL_GetWindowSizeInPixels(m_Window, &w, NULL);
			const float winW = static_cast<float>(w);
			const float scale = std::clamp(winW / 1280, 1.0f, 2.0f);
			int y;
			SDL_GetWindowPosition(m_Window, NULL, &y);
			ImGui::SetNextWindowSize({ 400.0f * scale, 0 });
			ImGui::SetNextWindowPos({ winW / 2 - std::clamp(200.0f * scale, 200.0f, 400.0f), 50 });
			ImGui::Begin("popup", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing);
			for (size_t i = 0; i < buttons.size(); i++) {
				if (whichToggled == i) {
					ImGui::PushStyleColor(ImGuiCol_Button, onColor);
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
				} else {
					ImGui::PushStyleColor(ImGuiCol_Button, offColor);
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
				}
				if(ImGui::Button(buttons[i].c_str())) {
					whichToggled = i;
				};
				ImGui::PopStyleColor(2);
				if (buttons.size() - 1 != i) {
					ImGui::SameLine();
				}
			}
			if (!ImGui::IsAnyItemActive() && !ImGui::IsWindowHovered())
				autoCompleteOn = false;
			ImGui::PushFont(Fonts::size25);
			std::string tempSearch = search;
			tempSearch.erase(std::remove_if(tempSearch.begin(), tempSearch.end(), [](unsigned char c) noexcept { return std::isspace(c); }), tempSearch.end());
			tempSearch.erase(std::remove(tempSearch.begin(), tempSearch.end(), '&'), tempSearch.end());
			
			if (tempSearch.size() > 1) {
				if (ImGui::IsKeyPressed(ImGuiKey_Enter, false)) {
					m_MessageManager.Publish("openSearch", std::make_pair(search,whichToggled));
					m_MessageManager.Publish("changeScene", std::string("SearchScene"));
					ImGui::CloseCurrentPopup();
				}
				if (tempSearch != currTerm) {
					currTerm = tempSearch;
					lastChange = std::chrono::system_clock::now();
					changed = true;
				}
				if (lastChange + std::chrono::milliseconds(250) < std::chrono::system_clock::now() && changed) {
					futureAutoCompleteResults = std::async(	std::launch::async, [this]() { return m_NetworkManager.GetAutoComplete(currTerm, std::to_string(user.userID)); });
					futureAllResults = std::async(std::launch::async, [this]() { return AllTermResults(); });
					futureProcessed = false;
					changed = false;
					allResults.clear();
				}
				if(!futureProcessed && futureAutoCompleteResults.wait_for(std::chrono::seconds(0)) == std::future_status::ready){
					autoCompleteResults.clear();
					if (futureAutoCompleteResults.valid());
						autoCompleteResults = futureAutoCompleteResults.get();
					futureProcessed = true;
					futureAllResults = std::async(std::launch::async, [this]() { return AllTermResults(); });
					futureAllProcessed = false;
				}
				if (whichToggled == 0) {
					if (!futureAllProcessed && futureAllResults.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
						if(futureAllResults.valid())
							allResults = futureAllResults.get();
						futureAllProcessed = true;
					}
					
					for (size_t i = 0; i < allResults.size() && i < 5; i++) {
						std::string term = allResults[i];
						if (ImGui::Selectable(term.c_str())) {
							T_INFO("selected: {0}", term);
							search = term;
							autoCompleteOn = false;
						}
					}
				} else {
					for (size_t i = 0; i < autoCompleteResults[buttons[whichToggled]].size(); i++) {
						std::string term = autoCompleteResults[buttons[whichToggled]][i];
						if (ImGui::Selectable(term.c_str())) {
							T_INFO("selected: {0}", term);
							search = term;
							autoCompleteOn = false;
						}
					}	
				}
			}
			ImGui::PopFont();
			ImGui::End();

		}
	}

	std::vector<std::string> CustomHeader::AllTermResults() {

		std::vector<std::string> allResults;
		for (auto& termResults : autoCompleteResults) {
			for (auto& result : termResults.second) {
				if(std::find(allResults.begin(), allResults.end(), result) == allResults.end())
					allResults.push_back(result);
			}
		}

		return allResults;
	}

}