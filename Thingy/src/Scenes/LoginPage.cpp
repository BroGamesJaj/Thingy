#include "tpch.h"
#include "LoginPage.h"

namespace Thingy {
	
	LoginPageScene::~LoginPageScene() {
	
	}
	
	void LoginPageScene::OnSwitch() {
		for (auto& module : modules) {
			module.second->OnLoad();
		}
	}
	
	void LoginPageScene::OnUpdate() {
		for (auto& module : modules) {
			module.second->OnUpdate();
		}
	}
	
	void LoginPageScene::OnRender() {
		for (auto& module : modules) {
			module.second->OnRender();
		}

	}
	
	void LoginPageScene::LayoutChanged() {
	}
	
	void LoginPageScene::UpdateLayout() {
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

	void LoginPageScene::SaveLayout() {
	}
}