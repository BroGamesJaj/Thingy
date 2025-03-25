#include "tpch.h"
#include "LoginScene.h"

namespace Thingy {

	LoginScene::LoginScene(MessageManager& messageManager) : m_MessageManager(messageManager) {
		m_MessageManager.Subscribe("change" + GetSceneName(), GetSceneName(), [this](const MessageData data) {
			BeforeSwitch();
			});
	};

	LoginScene::~LoginScene() {
	
	}
	
	void LoginScene::OnSwitch(std::unordered_map<std::string, std::variant<int, std::string>> newModuleState) {
		for (auto& module : modules) {
			module.second->OnLoad(-1);
		}
	}
	
	void LoginScene::OnUpdate() {
		for (auto& module : modules) {
			module.second->OnUpdate();
		}
	}
	
	uint16_t LoginScene::OnRender() {
		uint16_t sReturn = 0;
		for (auto& module : modules) {
			uint16_t mReturn = module.second->OnRender();
			sReturn |= mReturn;
		}
		return sReturn;
	}

	void LoginScene::BeforeSwitch() {
		for (auto& module : modules) {
			m_MessageManager.Publish("beforeSwitch" + module.first, GetSceneName());
		}
	}
	
	void LoginScene::UpdateLayout() {
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

	void LoginScene::SaveLayout() {
	}
}