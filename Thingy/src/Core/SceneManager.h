#pragma once
#include "Scene.h"
#include "Scenes\Scenes.h"

namespace Thingy {
	class SceneManager {
	public:
		SceneManager();
		~SceneManager();

		SceneManager(const SceneManager&) = delete;
		void operator=(const SceneManager&) = delete;

		void Listener();

		void ChangeScene(std::string newSceneName);

		std::shared_ptr<Scene>& GetScene(std::string sceneName) {
			T_ERROR("hello");
			auto it = scenes.find(sceneName);
			if (it != scenes.end()) {
				T_ERROR("Bigger what");
				return scenes[sceneName];
			}
			else {
				T_ERROR("{0} scene was not found", sceneName);
				throw ERROR_NOT_FOUND;
			}
			T_ERROR("What");
		}
		void GetScenes() {
			for (auto& scene : scenes) {
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
				activeScene = it->second;
				activeScene->OnSwitch();
			}
		}

		std::shared_ptr<Scene>& GetActiveScene() { return activeScene; }

	private:
		bool layoutChanged = false;
		std::unordered_map<std::string,std::shared_ptr<Scene>> scenes;
		std::shared_ptr<Scene> activeScene;

	};
}