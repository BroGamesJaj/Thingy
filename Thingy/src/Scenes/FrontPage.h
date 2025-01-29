#pragma once
#include "Core\Scene.h"

namespace Thingy {
	class FrontPageScene : Scene {
	public:
		~FrontPageScene();

		void OnSwitch() override;
		void OnUpdate() override;
		void OnRender() override;


		void LayoutChanged() override;
		void UpdateLayout() override;
		void SaveLayout() override;	

		SCENE_CLASS_NAME("FrontPage");
	private:

	};
}