#pragma once
#include "Scene.h"


namespace Thingy {
	class SceneManager {
	public:
		SceneManager();
		~SceneManager();

		SceneManager(const SceneManager&) = delete;
		void operator=(const SceneManager&) = delete;

		void Listener();

		void ChangeScene(std::string newSceneName);



	private:
		bool layoutChanged = false;
		std::unordered_map<std::string,std::unique_ptr<Scene>> Scenes;

	};
}