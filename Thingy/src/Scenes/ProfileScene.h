#pragma once
#include "Core\Scene.h"

namespace Thingy {
	class ProfileScene : public Scene {
	public:
		ProfileScene(MessageManager& messageManager);
		~ProfileScene();

		void OnSwitch(std::unordered_map<std::string, std::variant<int, std::string>> newModuleState) override;
		void OnUpdate() override;
		uint16_t OnRender() override;
		void BeforeSwitch() override;

		void UpdateLayout() override;
		void SaveLayout() override;

		SCENE_CLASS_NAME("ProfileScene");
	private:
		MessageManager& m_MessageManager;
	};
}