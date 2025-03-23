#pragma once
#include "Core\Scene.h"

namespace Thingy {
	class SearchScene : public Scene {
	public:
		SearchScene(MessageManager& messageManager);
		~SearchScene();

		void OnSwitch(std::unordered_map<std::string, std::variant<int, std::string>> newModuleState) override;
		void OnUpdate() override;
		uint16_t OnRender() override;
		void BeforeSwitch() override;

		void LayoutChanged() override;
		void UpdateLayout() override;
		void SaveLayout() override;

		SCENE_CLASS_NAME("SearchScene");
	private:
		MessageManager& m_MessageManager;
	};
}