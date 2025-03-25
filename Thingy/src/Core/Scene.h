#pragma once
#include "tpch.h"

#include "Module.h"
#include "Core\Managers\MessageManager.h"

namespace Thingy {

	//0 = Playlistlist
	//1 = main content
	//2 = player
	static std::array<int, 3> order = { 0, 1, 2 };

	typedef std::vector<std::pair<std::string, std::shared_ptr<Module>>> Modules;
	
	inline void LayoutChangeScene(std::string dragged, ImVec2 currentPos, Modules& modules, bool& changed) {
		T_TRACE("dragged: {0}", dragged);
		auto draggedModule = std::find_if(modules.begin(), modules.end(),
			[&dragged](const std::pair<std::string, std::shared_ptr<Module>>& module) {
				return module.first == dragged;
			});
		for (size_t i = 0; i < modules.size(); i++) {
			if (modules[i].first != dragged) {
				if (ImGui::FindWindowByName(modules[i].first.data())) {
					const ImVec2 winSize = ImGui::FindWindowByName(modules[i].first.data())->Size;
					const ImVec2 winPos = ImGui::FindWindowByName(modules[i].first.data())->Pos;
					if (currentPos.x > winPos.x && currentPos.x < winPos.x + winSize.x && currentPos.y > winPos.y && currentPos.y < winPos.y + winSize.y) {

						if (draggedModule != modules.end()) {
							std::swap(*draggedModule, modules[i]);
							changed = true;
							return;
						}
					}
				};
			}
		}
		changed = false;
	}


#define SCENE_CLASS_NAME(name) virtual std::string GetSceneName() const override { return name; }

	class Scene {
	public:
		virtual ~Scene() = default;

		virtual void OnSwitch(std::unordered_map<std::string, std::variant<int, std::string>> newModuleState) = 0;
		virtual void OnUpdate() = 0;
		virtual uint16_t OnRender() = 0;
		virtual void BeforeSwitch() = 0;

		virtual std::string GetSceneName() const = 0;
		const Modules& GetModules() const noexcept { return modules; };
		
		virtual void PushModule(std::shared_ptr<Module> module) {
			modules.push_back(std::make_pair(module->GetModuleName(),module));
		}

		virtual void PopModule(const std::string& moduleName) {
			const size_t beforeSize = modules.size();
			modules.erase(std::remove_if(modules.begin(), modules.end(), [&moduleName](const std::pair<std::string,std::shared_ptr<Module>>& module) {
				return module.first == moduleName;
				}), modules.end());
			if(modules.size() == beforeSize) {
				T_ERROR("Module {0} was not found for deletion in Scene {1}", moduleName, GetSceneName());
			}
		}

		virtual void UpdateLayout() = 0;
		virtual void SaveLayout() {
			for (size_t i = 0; i < modules.size(); i++) {
				if (modules[i].first == "PlayerModule") order[i] = 2;
				else if (modules[i].first == "PlaylistListModule") order[i] = 0;
				else order[i] = 1;
			}
		};

		virtual void LoadLayout(std::pair<std::string, std::shared_ptr<Module>>& module) {
			for (size_t i = 0; i < modules.size(); i++) {
				if (module.first == "PlaylistListModule" && order[i] == 0) {
					std::swap(module, modules[i]);
					break;
				}
				if (module.first == "PlayerModule" && order[i] == 2) {
					std::swap(module, modules[i]);
					break;
				}
			}
		}
 
	protected:
		Modules modules;
	};
}