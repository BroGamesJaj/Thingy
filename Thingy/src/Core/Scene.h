#pragma once
#include "tpch.h"

#include "Module.h"

namespace Thingy {

	typedef std::vector<std::pair<std::string, std::shared_ptr<Module>>> Modules;
	
	inline void LayoutChangeScene(std::string dragged, ImVec2 currentPos, Modules& modules, bool& changed) {
		T_TRACE("dragged: {0}", dragged);
		auto draggedModule = std::find_if(modules.begin(), modules.end(),
			[&dragged](const std::pair<std::string, std::shared_ptr<Module>>& module) {
				return module.first == dragged;
			});
		for (size_t i = 0; i < modules.size(); i++) {

			if (modules[i].first != dragged) {
				if (!ImGui::FindWindowByName(modules[i].first.data())) break;
				ImVec2 winSize = ImGui::FindWindowByName(modules[i].first.data())->Size;
				ImVec2 winPos = ImGui::FindWindowByName(modules[i].first.data())->Pos;
				if (currentPos.x > winPos.x && currentPos.x < winPos.x + winSize.x && currentPos.y > winPos.y && currentPos.y < winPos.y + winSize.y) {

					if (draggedModule != modules.end()) {
						std::swap(*draggedModule, modules[i]);
						changed = true;
						return;
					}
				}
			}
		}
		changed = false;
	}


#define SCENE_CLASS_NAME(name) virtual std::string GetSceneName() const override { return name; }

	class Scene {
	public:

		virtual ~Scene() = default;

		//Scene(const Scene&) = delete;
		//void operator=(const Scene&) = delete;

		//virtual void InitModules() const = 0;
		//virtual void CleanUpModules() const = 0;
		virtual void OnSwitch() = 0;
		virtual void OnUpdate() = 0;
		virtual uint16_t OnRender() = 0;
		virtual void BeforeSwitch() = 0;

		virtual std::string GetSceneName() const = 0;
		const Modules& GetModules() const { return modules; };
		
		virtual void PushModule(std::shared_ptr<Module> module) {
			modules.push_back(std::make_pair(module->GetModuleName(),module));
		}

		virtual void PopModule(const std::string& moduleName) {
			size_t beforeSize = modules.size();
			modules.erase(std::remove_if(modules.begin(), modules.end(), [&moduleName](const std::pair<std::string,std::shared_ptr<Module>>& module) {
				return module.first == moduleName;
				}), modules.end());
			if(modules.size() == beforeSize) {
				T_ERROR("Module {0} was not found for deletion in Scene {1}", moduleName, GetSceneName());
			}
		}
		
		virtual void LayoutChanged() = 0;
		virtual void UpdateLayout() = 0;
		virtual void SaveLayout() = 0;
 
	protected:
		Modules modules;
	};
}