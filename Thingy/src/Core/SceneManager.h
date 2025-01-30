#pragma once
#include "Scene.h"
#include "Scenes\Scenes.h"
#include "Log.h"

namespace Thingy {
	class SceneManager {
	public:
		SceneManager();
		~SceneManager();

		SceneManager(const SceneManager&) = delete;
		void operator=(const SceneManager&) = delete;

		void Listener();

		void ChangeScene(std::string newSceneName);

		void GetScenes() {
			for (auto scene : scenes) {
				T_INFO("{0}", scene.first);
			}
		}

		template <class T>
		void AddScene(std::shared_ptr<T> scene) {
			static_assert(std::is_base_of<Scene, T>::value, "T must derive from Scene");
			scenes[scene->GetSceneName()] = scene;
		}

		void SetActiveScene(const std::string& name) {
			auto it = scenes.find(name);
			if (it != scenes.end()) {
				activeScene = it->second.get();
			}
		}

	private:
		bool layoutChanged = false;
		std::unordered_map<std::string,std::shared_ptr<Scene>> scenes;
		Scene* activeScene;

	};
}