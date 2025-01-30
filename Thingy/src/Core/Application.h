#pragma once
#include "tpch.h"

#include "Renderer\SDLRenderer.h"
#include "AudioManager.h"
#include "NetworkManager.h"
#include "SceneManager.h"

namespace Thingy {
	class Application {
	public:
		Application();
		~Application();

		Application(const Application&) = delete;
		void operator=(const Application&) = delete;

		void SetupManagers();
		void SetupScenes();

		void Run();

		void EventLoop();

	private:
		bool Running = true;
		std::unique_ptr<SDLRenderer> renderer = nullptr;
		std::unique_ptr<AudioManager> audioManager = nullptr;
		std::unique_ptr<NetworkManager> networkManager = nullptr;
		std::unique_ptr<SceneManager> sceneManager = nullptr;
	};

}