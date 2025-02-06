#pragma once
#include "tpch.h"
#include "Core\Scene.h"
namespace Thingy {
	class AlbumScene : public Scene {
	public:

		~AlbumScene();

		void OnSwitch() override;
		void OnUpdate() override;
		uint16_t OnRender() override;

		void LayoutChanged() override;
		void UpdateLayout() override;
		void SaveLayout() override;

		SCENE_CLASS_NAME("AlbumScene");
	private:

	};
}