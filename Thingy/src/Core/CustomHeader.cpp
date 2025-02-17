#include "tpch.h"
#include "CustomHeader.h"

namespace Thingy {
	void CustomHeader(float& windowWidth, float& windowHeight, bool& done, SDL_Window& window, bool& fullscreen, bool& fullscreenChanged, std::string& search) {
		ImGui::SetNextWindowSize({ windowWidth, windowHeight });
		ImGui::SetNextWindowPos({ 0.0f, 0.0f });
		ImGui::Begin("Custom Header", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

		float winW = ImGui::GetWindowWidth();
		float scale = std::clamp(winW / 1280, 1.0f, 2.0f);
		ImGui::GetCurrentWindow()->DC.LayoutType = ImGuiLayoutType_Horizontal;
		ImGui::Button("image", { 40.0f, 40.0f });

		ImGui::SetCursorPosX(winW / 2 - std::clamp(200.0f * scale, 200.0f, 400.0f));
		ImGui::SetNextItemWidth(400.0f * scale);
		ImGui::PushFont(Fonts::size30);
		ImGui::InputText("##search", &search, 0, ResizeCallback, (void*)&search);
		ImGui::PopFont();
		ImGui::SetCursorPosX(winW - 15 - 40 - 5 - 40 - 5 - 40);
		if (ImGui::Button("_", { 40.0f, 30.0f }))
			SDL_MinimizeWindow(&window);

		//ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 10 - 40 - 5 - 40); // 10 padding right, width of X button, padding between buttons, width of button
		if (ImGui::Button("[]", { 40.0f, 30.0f })) {
			fullscreen = !fullscreen;
			fullscreenChanged = true;
		}
		//ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 10 - 40); // 10 padding right, width of button
		if (ImGui::Button("X", { 40.0f, 30.0f }))
			done = false;

		ImGui::GetCurrentWindow()->DC.LayoutType = ImGuiLayoutType_Vertical;
		ImGui::End();

	}
}