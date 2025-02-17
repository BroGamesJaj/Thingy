#include "tpch.h"
#include "CustomHeader.h"

namespace Thingy {

	CustomHeader::CustomHeader(std::unique_ptr<MessageManager>& messageManager, SDL_Window* window, std::string& searchField) : m_MessageManager(messageManager), m_Window(window), search(searchField) {

	}

	void CustomHeader::OnRender() {
		int w, h;
		SDL_GetWindowSizeInPixels(m_Window, &w, &h);
		ImGui::SetNextWindowSize({ static_cast<float>(w), static_cast<float>(h)});
		ImGui::SetNextWindowPos({ 0.0f, 0.0f });
		ImGui::Begin("Custom Header", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

		float winW = ImGui::GetWindowWidth();
		float scale = std::clamp(winW / 1280, 1.0f, 2.0f);
		ImGui::GetCurrentWindow()->DC.LayoutType = ImGuiLayoutType_Horizontal;
		ImGui::Button("image", { 40.0f, 40.0f });

		ImGui::SetCursorPosX(winW / 2 - std::clamp(200.0f * scale, 200.0f, 400.0f));
		ImGui::SetNextItemWidth(400.0f * scale);
		ImGui::PushFont(Fonts::size30);
		if (ImGui::InputText("##search", &search, 0, ResizeCallback, (void*)&search)) {
			
		}
		if (ImGui::BeginPopup("SearchPopup", ImGuiWindowFlags_NoMove)) {
			ImGui::Text("helllo");
			std::string temp = search + "hello";
			if (ImGui::Selectable(temp.c_str())) {
				strcpy(search.data(), temp.c_str());
				ImGui::CloseCurrentPopup();
			}
			
			ImGui::EndPopup();
		}
		ImGui::PopFont();
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
	}

	void CustomHeader::AutoComplete() {}

}