#include "tpch.h"
#include "PlayerModule.h"

#include "imgui.h"
#include "imgui_internal.h"
namespace Thingy {
	void PlayerModule::OnUpdate() {

	}

	void PlayerModule::OnRender() {
		ImGui::Begin(GetModuleName().data(), nullptr, defaultWindowFlags);
		ImGui::Button("image", { 300.0f, 300.0f });
		ImGui::Text("Thing name");
		ImGui::Button("back", { 30.0f, 30.0f });
		ImGui::SameLine();
		ImGui::Button("Play", { 30.0f, 30.0f });
		ImGui::SameLine();
		ImGui::Button("Forward", { 30.0f, 30.0f });
		ImGui::Text("Current");
		ImGui::SameLine();
		ImGui::SliderInt("time", &m_CurrentTime, 0, 200);
		
		ImGui::SliderInt("Volume", &m_AudioVolume, 0, 128);
		

		ImGui::End();
	}
}