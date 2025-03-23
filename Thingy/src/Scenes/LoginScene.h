#pragma once
#include "Core\Scene.h"

namespace Thingy {
	class LoginScene : public Scene {
	public:
		LoginScene(MessageManager& messageManager);
		~LoginScene();

		void OnSwitch(std::unordered_map<std::string, std::variant<int, std::string>> newModuleState) override;
		void OnUpdate() override;
		uint16_t OnRender() override;
		void BeforeSwitch() override;

		void LayoutChanged() override;
		void UpdateLayout() override;
		void SaveLayout() override;	

		SCENE_CLASS_NAME("LoginScene");
	private:
		MessageManager& m_MessageManager;
	};
}