#pragma once
#include "Core\Log.h"
#include "Core\Scene.h"
#include "Scenes\Scenes.h"
#include "MessageManager.h"

namespace Thingy {

	enum ActiveSceneType {
		OPEN,
		BACK,
		NEXT
	};
	
	struct SceneState {
		std::string sceneName;
		std::unordered_map<std::string, std::variant<int,std::string>> moduleState;

		SceneState(const std::string sName, const std::unordered_map<std::string, std::variant<int, std::string>> mState) {
			sceneName = sName;
			moduleState = mState;
		}
	};

	class SceneManager {
	public:
		SceneManager(MessageManager& messageManager);
		~SceneManager();

		SceneManager(const SceneManager&) = delete;
		void operator=(const SceneManager&) = delete;

		void ChangeScene(const std::string newSceneName, const ActiveSceneType type);

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

		bool SetActiveScene(const std::string& name, const ActiveSceneType type);

		std::shared_ptr<Scene>& GetActiveScene() { return activeScene; }

	private:
		bool layoutChanged = true;
		std::unordered_map<std::string,std::shared_ptr<Scene>> scenes;
		std::shared_ptr<Scene> activeScene;

		MessageManager& m_MessageManager;

		std::vector<SceneState> history;
		int currentSceneIndex = 0;


	};
}