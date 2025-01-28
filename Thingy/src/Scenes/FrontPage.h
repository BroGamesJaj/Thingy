#pragma once
#include "Core\Scene.h"

namespace Thingy {
	class FrontPageScene : Scene {
	public:
		~FrontPageScene();

		void OnSwitch() override;
		void OnUpdate() override;
		void OnRender() override;


		virtual void LayoutChanged() override;
		virtual void ChangeLayout() override;
		virtual void SaveLayout() override;	

		SCENE_CLASS_NAME("FrontPage");


	private:

	};
}