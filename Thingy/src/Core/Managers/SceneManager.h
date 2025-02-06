#pragma once
#include "Core\Log.h"
#include "Core\Scene.h"
#include "Scenes\Scenes.h"
#include "MessageManager.h"

namespace Thingy {
	class SceneManager {
	public:
		SceneManager(std::unique_ptr<MessageManager>& messageManager);
		~SceneManager();

		SceneManager(const SceneManager&) = delete;
		void operator=(const SceneManager&) = delete;

		void ChangeScene(std::string newSceneName);

		std::shared_ptr<Scene>& GetScene(std::string sceneName) {
			auto it = scenes.find(sceneName);
			if (it != scenes.end()) {
				return scenes[sceneName];
			}
			else {
				T_ERROR("SceneManager: {0} scene was not found", sceneName);
				throw ERROR_NOT_FOUND;
			}
		}
		void GetScenes() {
			for (auto& scene : scenes) {
				T_INFO("{0}", scene.first);
			}
		}

		bool GetLayoutChanged() { return layoutChanged; }
		void ResetLayoutChanged() { layoutChanged = false; }

		template <class T>
		void AddScene(std::shared_ptr<T> scene) {
			static_assert(std::is_base_of<Scene, T>::value, "T must derive from Scene");
			scenes[scene->GetSceneName()] = scene;
		}

		bool SetActiveScene(const std::string& name) {
			auto it = scenes.find(name);
			if (it != scenes.end()) {
				activeScene = it->second;
				activeScene->OnSwitch();
				return true;
			} else {
				T_ERROR("SceneManager: Scene not found");
				return false;
			}
		}

		std::shared_ptr<Scene>& GetActiveScene() { return activeScene; }

	private:
		bool layoutChanged = true;
		std::unordered_map<std::string,std::shared_ptr<Scene>> scenes;
		std::shared_ptr<Scene> activeScene;

		std::unique_ptr<MessageManager>& m_MessageManager;

	};
}