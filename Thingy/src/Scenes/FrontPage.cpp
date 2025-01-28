#include "tpch.h"
#include "FrontPage.h"

namespace Thingy {

	FrontPageScene::~FrontPageScene() {
	
	}
	
	void FrontPageScene::OnSwitch() {

	}
	
	void FrontPageScene::OnUpdate() {
		for (auto& module : modules) {
			module.second->OnUpdate();
		}
	}
	
	void FrontPageScene::OnRender() {
		for (auto& module : modules) {
			module.second->OnRender();
		}

	}
	
	void FrontPageScene::LayoutChanged() {
	}
	
	void FrontPageScene::ChangeLayout() {
	}

	void FrontPageScene::SaveLayout() {
	}
}