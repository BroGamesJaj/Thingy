#pragma once
#include "tpch.h"
#include "Core\Scene.h"

namespace Thingy {
	class AlbumScene : public Scene {
	public:
		AlbumScene(MessageManager& messageManager);
		~AlbumScene();

		void OnSwitch(std::unordered_map<std::string, std::variant<int, std::string>> newModuleState) override;
		void OnUpdate() override;
		uint16_t OnRender() override;
		void BeforeSwitch() override;

		void LayoutChanged() override;
		void UpdateLayout() override;
		void SaveLayout() override;

		SCENE_CLASS_NAME("AlbumScene");
	private:
		MessageManager& m_MessageManager;
		std::unordered_map<std::string, std::variant<int, std::string>> moduleStates;
	};
}