#pragma once
#include "tpch.h"
#include "Core\Scene.h"

namespace Thingy {
	class ArtistScene : public Scene {
	public:
		ArtistScene(std::unique_ptr<MessageManager>& messageManager);
		~ArtistScene();

		void OnSwitch(std::unordered_map<std::string, std::variant<int, std::string>> newModuleState) override;
		void OnUpdate() override;
		uint16_t OnRender() override;
		void BeforeSwitch() override;

		void LayoutChanged() override;
		void UpdateLayout() override;
		void SaveLayout() override;

		SCENE_CLASS_NAME("ArtistScene");
	private:
		std::unique_ptr<MessageManager>& m_MessageManager;
		std::unordered_map<std::string, std::variant<int, std::string>> moduleStates;
	};
}