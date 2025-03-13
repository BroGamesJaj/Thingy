#pragma once
#include "Core\Scene.h"

namespace Thingy {
	class FrontPageScene : public Scene {
	public:
		FrontPageScene(MessageManager& messageManager) : m_MessageManager(messageManager) {
			m_MessageManager.Subscribe("change" + GetSceneName(), GetSceneName(), [this](const MessageData data) {
				BeforeSwitch();
				});
		};
		~FrontPageScene();

		void OnSwitch(std::unordered_map<std::string, std::variant<int, std::string>> newModuleState) override;
		void OnUpdate() override;
		uint16_t OnRender() override;
		void BeforeSwitch() override;

		void LayoutChanged() override;
		void UpdateLayout() override;
		void SaveLayout() override;	

		SCENE_CLASS_NAME("FrontPage");
	private:
		MessageManager& m_MessageManager;
	};
}