#pragma once
#include "Core\Scene.h"

namespace Thingy {
	class LoginScene : public Scene {
	public:
		LoginScene(std::unique_ptr<MessageManager>& messageManager) : m_MessageManager(messageManager) {
			m_MessageManager->Subscribe("change" + GetSceneName(), GetSceneName(), [this](const MessageData data) {
				BeforeSwitch();
				});
		};
		~LoginScene();

		void OnSwitch(std::unordered_map<std::string, std::variant<int, std::string>> newModuleState) override;
		void OnUpdate() override;
		uint16_t OnRender() override;
		void BeforeSwitch() override;

		void LayoutChanged() override;
		void UpdateLayout() override;
		void SaveLayout() override;	

		SCENE_CLASS_NAME("Login");
	private:
		std::unique_ptr<MessageManager>& m_MessageManager;
	};
}