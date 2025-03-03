#include "tpch.h"
#include "ArtistScene.h"

namespace Thingy {
	ArtistScene::ArtistScene(std::unique_ptr<MessageManager>& messageManager) : m_MessageManager(messageManager) {
		m_MessageManager->Subscribe("change" + GetSceneName(), GetSceneName(), [this](const MessageData data) {
			BeforeSwitch();
			});
	};

	ArtistScene::~ArtistScene() {}

	void ArtistScene::OnSwitch(std::unordered_map<std::string, std::variant<int, std::string>> newModuleState) {
		for (auto& m : newModuleState) {
			if (std::holds_alternative<int>(m.second))
				std::cout << m.first << " " << std::get<int>(m.second) << std::endl;
			if (std::holds_alternative<std::string>(m.second))
				std::cout << m.first << " " << std::get<std::string>(m.second) << std::endl;
		}
		m_MessageManager->Subscribe("saveModuleState", GetSceneName(), [this](const MessageData data) {
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
			if (moduleStates.find(module.first) != moduleStates.end()) {
				module.second->OnLoad(moduleStates[module.first]);
			} else {
				module.second->OnLoad(-1);
			}
		}
	}

	void ArtistScene::OnUpdate() {

		for (auto& module : modules) {
			module.second->OnUpdate();
		}
	}

	uint16_t ArtistScene::OnRender() {
		std::string dragged = "";
		for (auto& module : modules) {
			uint16_t mReturn = module.second->OnRender();
			if (mReturn & BIT(1)) {
				dragged = module.first;
			}
		}
		if (dragged != "") {
			bool changed = false;
			LayoutChangeScene(dragged, ImGui::GetMousePos(), modules, changed);
			if (changed) UpdateLayout();
		}
		return 0;
	}


	void ArtistScene::BeforeSwitch() {
		for (auto& module : modules) {
			m_MessageManager->Publish("beforeSwitch" + module.first, GetSceneName());
		}
		for (auto& m : moduleStates) {
			if (std::holds_alternative<int>(m.second))
				std::cout << m.first << " " << std::get<int>(m.second) << std::endl;
			if (std::holds_alternative<std::string>(m.second))
				std::cout << m.first << " " << std::get<std::string>(m.second) << std::endl;
		}
		m_MessageManager->Publish("setHistory", moduleStates);
		m_MessageManager->UnSubscribe("saveModuleState", GetSceneName());
	}

	void ArtistScene::LayoutChanged() {
		if (!ImGui::GetCurrentContext()) {
			T_ERROR("ImGui context is not initialized!");
			return;
		}
		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id);
		ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport_size);
		float sum = 0;
		for (auto& module : modules) {
			sum += module.second->CurrentWidth();
		}
		float scale = viewport_size.x / sum;
		std::vector<ImGuiID> docks;
		for (auto& module : modules) {
			float ratio = module.second->CurrentWidth() * scale / viewport_size.x;
			docks.push_back(ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, ratio, nullptr, &dockspace_id));
		}

		for (size_t i = 0; i < modules.size(); i++) {
			if (modules[i].first == "PlayerModule") {
				ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(300, 500));
			}
			ImGui::DockBuilderDockWindow(modules[i].first.data(), docks[i]);
			if (modules[i].first == "PlayerModule") {
				ImGui::PopStyleVar();
			}
		}
		ImGui::DockBuilderFinish(dockspace_id);

	}

	void ArtistScene::UpdateLayout() {
		if (!ImGui::GetCurrentContext()) {
			T_ERROR("ImGui context is not initialized!");
			return;
		}
		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id);
		ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport_size);
		float sum = 0;
		for (auto& module : modules) {
			sum += module.second->CurrentWidth();
		}
		float scale = viewport_size.x / sum;
		std::vector<ImGuiID> docks;
		for (auto& module : modules) {
			float ratio = module.second->CurrentWidth() * scale / viewport_size.x;
			docks.push_back(ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, ratio, nullptr, &dockspace_id));
		}

		for (size_t i = 0; i < modules.size(); i++) {
			if (modules[i].first == "PlayerModule") {
				ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(300, 500));
			}
			ImGui::DockBuilderDockWindow(modules[i].first.data(), docks[i]);
			if (modules[i].first == "PlayerModule") {
				ImGui::PopStyleVar();
			}
		}
		ImGui::DockBuilderFinish(dockspace_id);
	}

	void ArtistScene::SaveLayout() {}
}