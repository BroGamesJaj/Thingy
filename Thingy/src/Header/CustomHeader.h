#pragma once
#include "tpch.h"
#include <SDL3\SDL.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include "Core\Managers\MessageManager.h"
#include "Core\Managers\NetworkManager.h"
#include "Core\Managers\ImageManager.h"
#include "Core\Managers\AuthManager.h"
#include "Core\Fonts.h"
#include "Core\HelperFunctions.h"

namespace Thingy {

	class CustomHeader {
	public:
		CustomHeader(MessageManager& messageManager, NetworkManager& networkManager, ImageManager& imageManager, AuthManager& authManager, SDL_Window* window, std::string& searchField);

		void OnRender();

		void AutoComplete();




	private:
		std::vector<std::string> AllTermResults();

		MessageManager& m_MessageManager;
		NetworkManager& m_NetworkManager;
		ImageManager& m_ImageManager;
		AuthManager& m_AuthManager;

		SDL_Window* m_Window;
		bool loggedIn = false;
		const User& user;
		std::unique_ptr<SDL_Texture, SDL_TDeleter> appTexture;
		std::unique_ptr<SDL_Texture, SDL_TDeleter> pfpTexture;

		bool autoCompleteOn = false;
		bool futureProcessed = true;
		bool futureAllProcessed = true;
		int whichToggled = 0;
		std::vector<std::string> buttons = { "all", "tags", "tracks", "albums", "artists", "playlists"};
		std::string& search;
		std::string	currTerm = "";
		std::unordered_map<std::string, std::vector<std::string>> autoCompleteResults;
		std::future<std::unordered_map<std::string, std::vector<std::string>>> futureAutoCompleteResults;
		std::future<std::vector<std::string>> futureAllResults;
		std::vector<std::string> allResults;

		std::chrono::system_clock::time_point lastChange;
		bool changed = false;
	};

}