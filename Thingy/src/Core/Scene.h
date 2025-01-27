#pragma once
#include "tpch.h"
#include "Log.h"

#include "Module.h"

namespace Thingy {

	typedef std::map<std::string, std::shared_ptr<Module>> ModuleDict;

#define SCENE_CLASS_NAME(name) virtual std::string GetSceneName() const override { return name; }

	class Scene {
	public:

		virtual ~Scene() = default;

		Scene(const Scene&) = delete;
		void operator=(const Scene&) = delete;

		//virtual void InitModules() const = 0;
		//virtual void CleanUpModules() const = 0;
		virtual void OnSwitch() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnRender() = 0;

		virtual std::string GetSceneName() const = 0;
		const ModuleDict& GetModules() const { return modules; };
		
		void PushModule(std::shared_ptr<Module> module) {
			modules.emplace(module->GetModuleName(), module);
			LayoutChanged();
		}

		void PopModule(std::string moduleName) {
			if (auto search = modules.find(moduleName); search != modules.end())
				modules.erase(moduleName);
			else {
				T_ERROR("Module {0} was not found for deletion in Scene {1}", moduleName, GetSceneName());
			}
			LayoutChanged();
		}
		
		virtual void LayoutChanged() = 0;
		virtual void ChangeLayout() = 0;
		virtual void SaveLayout() = 0;
 
	protected:
		ModuleDict modules;
	};
}