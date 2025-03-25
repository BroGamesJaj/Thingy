#pragma once
#include "Core\Scene.h"

namespace Thingy {
	class FrontPageScene : public Scene {
	public:
		FrontPageScene(MessageManager& messageManager);
		~FrontPageScene();

		void OnSwitch(std::unordered_map<std::string, std::variant<int, std::string>> newModuleState) override;
		void OnUpdate() override;
		uint16_t OnRender() override;
		void BeforeSwitch() override;

		void UpdateLayout() override;

		SCENE_CLASS_NAME("FrontPage");
	private:
		MessageManager& m_MessageManager;
	};
}