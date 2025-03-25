#include "tpch.h"
#include "PlaylistScene.h"

namespace Thingy {
	PlaylistScene::PlaylistScene(MessageManager& messageManager) : m_MessageManager(messageManager) {
		m_MessageManager.Subscribe("change" + GetSceneName(), GetSceneName(), [this](const MessageData data) {
			BeforeSwitch();
			});
	};

	PlaylistScene::~PlaylistScene() {}

	void PlaylistScene::OnSwitch(std::unordered_map<std::string, std::variant<int, std::string>> newModuleState) {
		for (auto& m : newModuleState) {
			if (std::holds_alternative<int>(m.second))
				std::cout << m.first << " " << std::get<int>(m.second) << std::endl;
			if (std::holds_alternative<std::string>(m.second))
				std::cout << m.first << " " << std::get<std::string>(m.second) << std::endl;
		}
		m_MessageManager.Subscribe("saveModuleState", GetSceneName(), [this](const MessageData data) {
			T_ERROR("saveModuleState");
			if (data.type() == typeid(std::pair<std::string, std::variant<int, std::string>>)) {
				std::pair<std::string, std::variant<int, std::string>> pair = std::any_cast<std::pair<std::string, std::variant<int, std::string>>>(data);
				moduleStates.emplace(pair.first, pair.second);
			} else {
				T_ERROR("There was a problem saving module state!");
			}
			});
		moduleStates.clear();
		moduleStates = newModuleState;
		for (auto& module : modules) {
			LoadLayout(module);
			if (moduleStates.find(module.first) != moduleStates.end()) {
				module.second->OnLoad(moduleStates[module.first]);
			} else {
				module.second->OnLoad(-1);
			}
		}
	}

	void PlaylistScene::OnUpdate() {

		for (auto& module : modules) {
			module.second->OnUpdate();
		}
	}

	uint16_t PlaylistScene::OnRender() {
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


	void PlaylistScene::BeforeSwitch() {
		SaveLayout();
		for (auto& module : modules) {
			m_MessageManager.Publish("beforeSwitch" + module.first, GetSceneName());
		}
		for (auto& m : moduleStates) {
			if (std::holds_alternative<int>(m.second))
				std::cout << m.first << " " << std::get<int>(m.second) << std::endl;
			if (std::holds_alternative<std::string>(m.second))
				std::cout << m.first << " " << std::get<std::string>(m.second) << std::endl;
		}
		m_MessageManager.Publish("setHistory", moduleStates);
		m_MessageManager.UnSubscribe("saveModuleState", GetSceneName());
	}

	void PlaylistScene::LayoutChanged() {
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

	void PlaylistScene::UpdateLayout() {
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

	void PlaylistScene::SaveLayout() {}
}