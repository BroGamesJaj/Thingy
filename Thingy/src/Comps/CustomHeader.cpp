#include "tpch.h"
#include "CustomHeader.h"

namespace Thingy {

	CustomHeader::CustomHeader(std::unique_ptr<MessageManager>& messageManager, std::unique_ptr<NetworkManager>& networkManager, std::unique_ptr<ImageManager>& imageManager, std::unique_ptr<AuthManager>& authManager, SDL_Window* window, std::string& searchField) : m_MessageManager(messageManager), m_NetworkManager(networkManager), m_ImageManager(imageManager), m_AuthManager(authManager), m_Window(window), search(searchField) {
		m_MessageManager->Subscribe("loggedIn", "customHeader", [this](const MessageData data) {
			if (data.type() == typeid(bool)) {
				loggedIn = std::any_cast<bool>(data);
				if (loggedIn) {
					const User& user = m_AuthManager->GetUser();
					if (user.pfpBuffer.empty()) {
						pfpTexture = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager->GetDefaultArtistImage());
					} else {
						pfpTexture = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager->GetTextureFromImage(Image(m_AuthManager->GetUser().pfpBuffer)));
					}
				} else {
					pfpTexture = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager->GetDefaultArtistImage());
				}

			}
			});
	}

	void CustomHeader::OnRender() {
		int w, h;
		SDL_GetWindowSizeInPixels(m_Window, &w, &h);
		ImGui::SetNextWindowSize({ static_cast<float>(w), static_cast<float>(h) });
		ImGui::SetNextWindowPos({ 0.0f, 0.0f });
		ImGui::Begin("Custom Header", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

		float winW = ImGui::GetWindowWidth();
		float scale = std::clamp(winW / 1280, 1.0f, 2.0f);
		ImGui::GetCurrentWindow()->DC.LayoutType = ImGuiLayoutType_Horizontal;
		ImGui::Button("image", { 40.0f, 40.0f });
		if (ImGui::IsItemClicked()) {
			m_MessageManager->Publish("homeButton", std::string("FrontPage"));
		}
		ImGui::Button("back", { 40.0f, 40.0f });
		if (ImGui::IsItemClicked()) {
			T_INFO("back");
			m_MessageManager->Publish("previousScene", "");
		}
		ImGui::Button("next", { 40.0f, 40.0f });
		if (ImGui::IsItemClicked()) {
			T_INFO("next");
			m_MessageManager->Publish("nextScene", "");
		}
		ImGui::SetCursorPosX(winW / 2 - std::clamp(200.0f * scale, 200.0f, 400.0f));
		ImGui::SetNextItemWidth(400.0f * scale);
		ImGui::PushFont(Fonts::size30);
		ImGui::InputText("##search", &search, 0, ResizeCallback, (void*)&search);
		if (ImGui::IsItemFocused()) {
			autoCompleteOn = true;
		}
		ImGui::PopFont();
		/*
		if (ImGui::BeginPopup("SearchPopup", ImGuiWindowFlags_NoMove)) {
			ImGui::Text("helllo");
			std::string temp = search + "hello";
			if (ImGui::Selectable(temp.c_str())) {
				strcpy(search.data(), temp.c_str());
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		*/
		if (ImGui::Button("Queue")) {
			m_MessageManager->Publish("changeQueueOpen", "");
		};
		if (loggedIn) {
			if(ImGui::ImageButton("##pfp", (ImTextureID)(intptr_t)pfpTexture.get(), ImVec2(40.0f, 40.0f))) {
				ImGui::OpenPopup("profile");
			};
			
			if (ImGui::BeginPopup("profile", ImGuiWindowFlags_NoMove)) {
				if (ImGui::Button("Details")) {
					m_MessageManager->Publish("changeScene", std::string("ProfileScene"));
					ImGui::CloseCurrentPopup();
				};
				if (ImGui::Button("Logout")) {
					T_INFO("Logout");
					m_MessageManager->Publish("logout", "");
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		} else {
			if (ImGui::Button("Login")) {
				m_MessageManager->Publish("changeScene", std::string("LoginScene"));
			}
		}
		ImGui::SetCursorPosX(winW - 15 - 40 - 5 - 40 - 5 - 40);
		if (ImGui::Button("_", { 40.0f, 30.0f }))
			m_MessageManager->Publish("minimize", 0);

		//ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 10 - 40 - 5 - 40); // 10 padding right, width of X button, padding between buttons, width of button
		if (ImGui::Button("[]", { 40.0f, 30.0f })) {
			m_MessageManager->Publish("changeFullscreen", 0);
		}
		//ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 10 - 40); // 10 padding right, width of button
		if (ImGui::Button("X", { 40.0f, 30.0f }))
			m_MessageManager->Publish("closeWindow", 0);

		ImGui::GetCurrentWindow()->DC.LayoutType = ImGuiLayoutType_Vertical;
		ImGui::End();

		AutoComplete();
	}

	void CustomHeader::AutoComplete() {
		ImVec4 onColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImVec4 offColor = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
		if (autoCompleteOn) {
			int w;
			SDL_GetWindowSizeInPixels(m_Window, &w, NULL);
			float winW = static_cast<float>(w);
			float scale = std::clamp(winW / 1280, 1.0f, 2.0f);
			int y;
			SDL_GetWindowPosition(m_Window, NULL, &y);
			ImGui::SetNextWindowSize({ 400.0f * scale, 0 });
			ImGui::SetNextWindowPos({ winW / 2 - std::clamp(200.0f * scale, 200.0f, 400.0f), 50 });
			ImGui::Begin("popup", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing);
			
			for (size_t i = 0; i < buttons.size(); i++) {
				if (whichToggled == i) {
					ImGui::PushStyleColor(ImGuiCol_Button, onColor);
					ImGui::PushStyleColor(ImGuiCol_Text, offColor);
				} else {
					ImGui::PushStyleColor(ImGuiCol_Button, offColor);
					ImGui::PushStyleColor(ImGuiCol_Text, onColor);
				}
				if(ImGui::Button(buttons[i].data())) {
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
			tempSearch.erase(std::remove_if(tempSearch.begin(), tempSearch.end(), [](unsigned char c) { return std::isspace(c); }), tempSearch.end());
			tempSearch.erase(std::remove(tempSearch.begin(), tempSearch.end(), '&'), tempSearch.end());
			
			if (tempSearch.size() > 1) {
				
				if (tempSearch != currTerm) {
					currTerm = tempSearch;
					lastChange = std::chrono::system_clock::now();
					changed = true;
				}
				if (lastChange + std::chrono::milliseconds(250) < std::chrono::system_clock::now() && changed) {

					futureAutoCompleteResults = std::async(	std::launch::async, [this]() { return m_NetworkManager->GetAutoComplete(currTerm); });
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
						std::string term = allResults[i].first;
						if (ImGui::Selectable(term.data())) {
							T_INFO("selected: {0}", term);
							search = term;
							autoCompleteOn = false;
						}
					}
				} else {
					for (size_t i = 0; i < autoCompleteResults[buttons[whichToggled]].size(); i++) {
						std::string term = autoCompleteResults[buttons[whichToggled]][i].first;
						if (ImGui::Selectable(term.data())) {
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

	std::vector<std::pair<std::string, int>> CustomHeader::AllTermResults() {
		std::vector<std::pair<std::string, int>> sorted;
		for (auto& termResults : autoCompleteResults) {
			for (auto& result : termResults.second) {
				if(std::find(sorted.begin(), sorted.end(), result) == sorted.end())
					sorted.push_back(result);
			}
		}
		std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
			return a.second < b.second;
			});

		return sorted;
	}

}