#include "tpch.h"
#include "SceneManager.h"


namespace Thingy {
	SceneManager::SceneManager(std::unique_ptr<MessageManager>& messageManager) : m_MessageManager(messageManager) {
		m_MessageManager->Subscribe("changeScene", "sceneManager", [this](MessageData name) {
			std::visit([this](auto&& value) {
				using T = std::decay_t<decltype(value)>;
				if constexpr (std::is_same_v<T, std::string>) {
					ChangeScene(value);
				} else {
					T_ERROR("SceneManager: Invalid data type for scene name");
				}
				}, name);
			});
	}
	
	SceneManager::~SceneManager() {
	}
	
	
	void SceneManager::ChangeScene(std::string newSceneName) {
		if (SetActiveScene(newSceneName)) {
			layoutChanged = true;
		}
	}
}