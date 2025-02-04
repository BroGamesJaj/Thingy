#include "tpch.h"
#include "LoginScene.h"

namespace Thingy {
	
	LoginScene::~LoginScene() {
	
	}
	
	void LoginScene::OnSwitch() {
		for (auto& module : modules) {
			module.second->OnLoad();
		}
	}
	
	void LoginScene::OnUpdate() {
		for (auto& module : modules) {
			module.second->OnUpdate();
		}
	}
	
	uint16_t LoginScene::OnRender() {
		for (auto& module : modules) {
			module.second->OnRender();
		}
		return 0;
	}
	
	void LoginScene::LayoutChanged() {
	}
	
	void LoginScene::UpdateLayout() {
		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id);

		ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport_size);

		std::vector<ImGuiID> docks;
		for (auto module : modules) {
			float ratio = module.second->CurrentWidth() / viewport_size.x;
			docks.push_back(ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, ratio, nullptr, &dockspace_id));
		}

		for (size_t i = 0; i < modules.size(); i++) {
			ImGui::DockBuilderDockWindow(modules[i].first.data(), docks[i]);
		}
		ImGui::DockBuilderFinish(dockspace_id);

	}

	void LoginScene::SaveLayout() {
	}
}