#include "tpch.h"
#include "SceneManager.h"


namespace Thingy {
	SceneManager::SceneManager(std::unique_ptr<MessageManager>& messageManager) : m_MessageManager(messageManager) {
		m_MessageManager->Subscribe("changeScene", "sceneManager", [this](const MessageData data) {
			
			if (data.type() == typeid(std::string)){
				std::string name = std::any_cast<std::string>(data);
				if (GetActiveScene()->GetSceneName() != name) {
					ChangeScene(name, OPEN);
				}
			} else {
				T_ERROR("SceneManager: Invalid data type for scene name");
			}
				
			});

		m_MessageManager->Subscribe("homeButton", "sceneManager", [this](const MessageData data) {
				if (data.type() == typeid(std::string)) {
					std::string name = std::any_cast<std::string>(data);
					if (GetActiveScene()) {
						if (GetActiveScene()->GetSceneName() != name) {
						
							m_MessageManager->Publish("change" + GetActiveScene()->GetSceneName(), "OPEN");
							ChangeScene(name, OPEN);
							T_INFO("Went to FrontPage");
						} else {
							T_INFO("Currently on frontpage");
						}
					} else {
						ChangeScene(name, OPEN);
					}
				} else {
					T_ERROR("SceneManager: Invalid data type for scene change");
				}
			
			});

		m_MessageManager->Subscribe("previousScene", "sceneManager", [this](const MessageData data) {
			if (history.size() < 2 || currentSceneIndex == 0) {
				T_INFO("Can't go back.");
				return;
			}
			m_MessageManager->Publish("change" + GetActiveScene()->GetSceneName(), "BACK");
			ChangeScene(history[currentSceneIndex - 1].sceneName, BACK);
			});

		m_MessageManager->Subscribe("nextScene", "sceneManager", [this](const MessageData data) {
			if (currentSceneIndex == history.size() - 1) {
				T_INFO("Can't go Forward.");
				return;
			}
			m_MessageManager->Publish("change" + GetActiveScene()->GetSceneName(), "NEXT");
			ChangeScene(history[currentSceneIndex + 1].sceneName, NEXT);
			});

		m_MessageManager->Subscribe("setHistory", "sceneManager", [this](const MessageData data) {
			if (data.type() == typeid(std::unordered_map<std::string, std::variant<int, std::string>>)) {
				std::unordered_map<std::string, std::variant<int, std::string>> moduleStates = std::any_cast<std::unordered_map<std::string, std::variant<int, std::string>>>(data);
				history[currentSceneIndex].moduleState.clear();
				history[currentSceneIndex].moduleState = moduleStates;
			} else {
				T_ERROR("SceneManager: Invalid data type for setHistory 1");
			}
			
			});


	}
	
	SceneManager::~SceneManager() {
	}
	
	
	void SceneManager::ChangeScene(const std::string newSceneName, const ActiveSceneType type) {
		T_INFO("CSI: {0}", currentSceneIndex);
		if (SetActiveScene(newSceneName, type)) {
			if (type == OPEN) {
				if (!history.empty()) {
					if(history.begin() + currentSceneIndex + 1 < history.end())
					history.erase(history.begin() + currentSceneIndex + 1, history.end());
				}
				history.push_back(SceneState(newSceneName, {}));
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

	bool SceneManager::SetActiveScene(const std::string& name, const ActiveSceneType type) {
		auto it = scenes.find(name);
		if (it != scenes.end()) {
			
			activeScene = it->second;
			if (!history.empty()) {
				if (type == BACK) activeScene->OnSwitch(history[currentSceneIndex - 1].moduleState);
				if (type == NEXT) activeScene->OnSwitch(history[currentSceneIndex + 1].moduleState);
				if (type == OPEN) activeScene->OnSwitch(history[currentSceneIndex].moduleState);
			} else {
				activeScene->OnSwitch({});
			}
			return true;
		} else {
			T_ERROR("SceneManager: Scene not found");
			return false;
		}
	}
}