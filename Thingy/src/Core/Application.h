#pragma once
#include "tpch.h"

#include "Renderer\SDLRenderer.h"
#include "Managers\AudioManager.h"
#include "Managers\NetworkManager.h"
#include "Managers\ImageManager.h"
#include "Managers\SceneManager.h"
#include "Managers\MessageManager.h"

#include "Image.h"

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
		
		std::unordered_map<std::string, SDL_Cursor*> customCursors;

		std::unique_ptr<SDLRenderer> renderer = nullptr;
		std::unique_ptr<AudioManager> audioManager = nullptr;
		std::unique_ptr<NetworkManager> networkManager = nullptr;
		std::unique_ptr<ImageManager> imageManager = nullptr;
		std::unique_ptr<SceneManager> sceneManager = nullptr;
		std::unique_ptr<MessageManager> messageManager = nullptr;

		std::unordered_map<std::string, std::shared_ptr<Module>> storedModules;

	};

}