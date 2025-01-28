#include "tpch.h"
#include "PopularsModule.h"

namespace Thingy {




	void PopularsModule::OnUpdate() {
	}
	void PopularsModule::Window(std::string title) {
		ImGui::Begin(title.data(), nullptr, defaultWindowFlags);
		ImVec2 winSize = ImGui::GetWindowSize();
		float centering = (winSize.x - 475) / 2;
		bool collapse = winSize.x > 1000 ? true : false;
		if (collapse) {

			ImGui::Text("Weekly Top Tracks");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			ImGui::Text("Monthly Top Tracks");
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi", { 85.0f, 85.0f });
				ImGui::SameLine();
			}
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi2", { 85.0f, 85.0f });
				if (i != 4)
					ImGui::SameLine();
			}
			ImGui::Text("Weekly Top Albums");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			ImGui::Text("Monthly Top Albums");
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi3", { 85.0f, 85.0f });
				ImGui::SameLine();
			}
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi4", { 85.0f, 85.0f });
				if (i != 4)
					ImGui::SameLine();
			}
			ImGui::Text("Weekly Top Artists");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			ImGui::Text("Monthly Top Artists");
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi5", { 85.0f, 85.0f });
				ImGui::SameLine();
			}
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 + 5);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi6", { 85.0f, 85.0f });
				if (i != 4)
					ImGui::SameLine();
			}
		} else {
			
			ImGui::SetCursorPosX(centering);
			ImGui::Text("Weekly Top Tracks");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi", { 85.0f, 85.0f });
				if (i != 4)
				ImGui::SameLine();
			}

			ImGui::SetCursorPosX(centering);
			ImGui::Text("Monthly Top Tracks");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi2", { 85.0f, 85.0f });
				if (i != 4)
					ImGui::SameLine();
			}

			ImGui::SetCursorPosX(centering);
			ImGui::Text("Weekly Top Albums");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi3", { 85.0f, 85.0f });
				if (i != 4)
				ImGui::SameLine();
			}
			
			ImGui::SetCursorPosX(centering);
			ImGui::Text("Monthly Top Albums");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi4", { 85.0f, 85.0f });
				if (i != 4)
					ImGui::SameLine();
			}

			ImGui::SetCursorPosX(centering);
			ImGui::Text("Weekly Top Artists");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi5", { 85.0f, 85.0f });
				if (i != 4)
				ImGui::SameLine();
			}

			ImGui::SetCursorPosX(centering);
			ImGui::Text("Monthly Top Artists");
			ImGui::SetCursorPosX(centering);
			for (size_t i = 0; i < 5; i++) {
				ImGui::Button("hi6", { 85.0f, 85.0f });
				if (i != 4)
					ImGui::SameLine();
			}
		}
		ImGui::Text("window size: (%.1f, %.1f)", ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
		ImGui::End();
	}
	void PopularsModule::OnRender() {
		Window(GetModuleName().data());
	}
}