#include "tpch.h"
#include "SceneManager.h"


namespace Thingy {
	SceneManager::SceneManager(std::unique_ptr<MessageManager>& messageManager) : m_MessageManager(messageManager) {
		m_MessageManager->Subscribe("changeScene", "sceneManager", [this](MessageData name) {
			std::visit([this](auto&& value) {
				using T = std::decay_t<decltype(value)>;
				if constexpr (std::is_same_v<T, std::string>) {
					if (value == "albumScene") {
						
					}
					ChangeScene(value, OPEN);
				} else {
					T_ERROR("SceneManager: Invalid data type for scene name");
				}
				}, name);
			});

		m_MessageManager->Subscribe("homeButton", "sceneManager", [this](MessageData dunno) {
			if (GetActiveScene()->GetSceneName() != "FrontPage") {
				ChangeScene("FrontPage", OPEN);
				T_INFO("Went to FrontPage");
			} else {
				T_INFO("Currently on frontpage");
			}
			});

		m_MessageManager->Subscribe("previousScene", "sceneManager", [this](MessageData dunno) {
			if (history.size() < 2 || currentSceneIndex == 0) {
				T_INFO("Can't go back.");
				return;
			}
			ChangeScene(history[currentSceneIndex - 1].sceneName, BACK);
			});

		m_MessageManager->Subscribe("nextScene", "sceneManager", [this](MessageData dunno) {
			if (currentSceneIndex == history.size() - 1) {
				T_INFO("Can't go Forward.");
				return;
			}
			ChangeScene(history[currentSceneIndex + 1].sceneName, NEXT);
			});
	}
	
	SceneManager::~SceneManager() {
	}
	
	
	void SceneManager::ChangeScene(const std::string newSceneName, const ActiveSceneType type) {
		for (auto i : history) {
			T_INFO("history: {0}", i);
		}
		T_INFO("CSI: {0}", currentSceneIndex);
		if (SetActiveScene(newSceneName)) {
			if (type == OPEN) {
				if (!history.empty()) {
					history.erase(history.begin() + currentSceneIndex + 1, history.end());
				}
				history.push_back(SceneState("asd", {}));
				currentSceneIndex = history.size() - 1;
			}
			if (type == BACK) {
				currentSceneIndex--;
			}
			if (type == NEXT) {
				currentSceneIndex++;
			}
			layoutChanged = true;
		}
	}
	bool SceneManager::SetActiveScene(const std::string& name) {
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
}