#pragma once
#include "Core\Scene.h"

namespace Thingy {
	class FrontPageScene : public Scene {
	public:
		
		~FrontPageScene();

		void OnSwitch() override;
		void OnUpdate() override;
		uint16_t OnRender() override;

		void LayoutChanged() override;
		void UpdateLayout() override;
		void SaveLayout() override;	

		SCENE_CLASS_NAME("FrontPage");
	private:

	};
}