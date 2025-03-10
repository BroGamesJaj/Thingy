#pragma once
#include "tpch.h"
#include "Fonts.h"

#include "Renderer\SDLRenderer.h"
#include "Managers\AudioManager.h"
#include "Managers\NetworkManager.h"
#include "Managers\ImageManager.h"
#include "Managers\SceneManager.h"
#include "Managers\MessageManager.h"
#include "Managers\AuthManager.h"

#include "Image.h"
#include "Comps\CustomHeader.h"

namespace Thingy {
	class Application {
	public:
		Application();
		~Application();

		Application(const Application&) = delete;
		void operator=(const Application&) = delete;

		void SetupManagers();
		void SetupScenes();

		void StartSubscriptions();

		void Run();

		void EventLoop();

	private:
		bool Running = true;
		std::string searchTerm = "";
		std::unordered_map<std::string, SDL_Cursor*> customCursors;

		std::unique_ptr<SDLRenderer> renderer = nullptr;
		std::unique_ptr<AudioManager> audioManager = nullptr;
		std::unique_ptr<NetworkManager> networkManager = nullptr;
		std::unique_ptr<ImageManager> imageManager = nullptr;
		std::unique_ptr<SceneManager> sceneManager = nullptr;
		std::unique_ptr<MessageManager> messageManager = nullptr;
		std::unique_ptr<AuthManager> authManager = nullptr;

		std::unordered_map<std::string, std::shared_ptr<Module>> storedModules;
		std::unique_ptr<CustomHeader> customHeader = nullptr;

	};

}