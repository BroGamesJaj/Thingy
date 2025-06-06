#include "tpch.h"
#include "FrontPageScene.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace Thingy {
	FrontPageScene::FrontPageScene(MessageManager& messageManager) : m_MessageManager(messageManager) {
		m_MessageManager.Subscribe("change" + GetSceneName(), GetSceneName(), [this](const MessageData data) {
			BeforeSwitch();
			});
	};

	FrontPageScene::~FrontPageScene() {
	
	}
	
	void FrontPageScene::OnSwitch(std::unordered_map<std::string, std::variant<int, std::string>> newModuleState) {
		for (auto& module : modules) {
			LoadLayout(module);
			module.second->OnLoad(-1);
		}
	}
	
	void FrontPageScene::OnUpdate() {
		for (auto& module : modules) {
			module.second->OnUpdate();
		}
	}
	
	uint16_t FrontPageScene::OnRender() {
		std::string dragged = "";
		uint16_t sReturn = 0;
		for (auto& module : modules) {
			uint16_t mReturn = module.second->OnRender();
			sReturn |= mReturn;
			if (mReturn & BIT(1)) {
				dragged = module.first;
			}
		}
		if (dragged != "") {
			bool changed = false;
			LayoutChangeScene(dragged, ImGui::GetMousePos(), modules, changed);
			if (changed) UpdateLayout();
		}
		return sReturn;
	}

	void FrontPageScene::BeforeSwitch() {
		T_INFO("before Switch");
		SaveLayout();
		for (auto& module : modules) {
			m_MessageManager.Publish("beforeSwitch" + module.first, GetSceneName());
		}
	}
	
	void FrontPageScene::UpdateLayout() {
		if (!ImGui::GetCurrentContext()) {
			T_ERROR("ImGui context is not initialized!");
			return;
		}
		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		
		ImGui::DockBuilderRemoveNode(dockspace_id);
		
		ImGui::DockBuilderAddNode(dockspace_id);
		ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport_size);
		
		std::vector<ImGuiID> docks;
		for (size_t i = 0; i < modules.size(); i++) {
			float sum = 0;
			for (size_t j = i; j < modules.size(); j++) {
				sum += modules[j].second->DefaultSize();
			}
			const float ratio = modules[i].second->DefaultSize() / sum;
			docks.push_back(ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, ratio, nullptr, &dockspace_id));
		}

		for (size_t i = 0; i < modules.size(); i++) {
			ImGui::DockBuilderDockWindow(modules[i].first.data(), docks[i]);
		}
		ImGui::DockBuilderFinish(dockspace_id);

	}
}