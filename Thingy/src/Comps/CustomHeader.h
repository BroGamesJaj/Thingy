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
		CustomHeader(std::unique_ptr<MessageManager>& messageManager, std::unique_ptr<NetworkManager>& netwokrManager, std::unique_ptr<ImageManager>& imageManager, std::unique_ptr<AuthManager>& authManager, SDL_Window* window, std::string& searchField);

		void OnRender();

		void AutoComplete();




	private:
		struct SDL_TDeleter { void operator()(SDL_Texture* p) { SDL_DestroyTexture(p); } };

		std::vector<std::pair<std::string, int>> AllTermResults();

		std::unique_ptr<MessageManager>& m_MessageManager;
		std::unique_ptr<NetworkManager>& m_NetworkManager;
		std::unique_ptr<ImageManager>& m_ImageManager;
		std::unique_ptr<AuthManager>& m_AuthManager;

		SDL_Window* m_Window;
		bool loggedIn = false;
		const User& user;
		std::unique_ptr<SDL_Texture, SDL_TDeleter> appTexture;
		std::unique_ptr<SDL_Texture, SDL_TDeleter> pfpTexture;

		bool autoCompleteOn = false;
		bool futureProcessed = true;
		bool futureAllProcessed = true;
		int whichToggled = 0;
		std::vector<std::string> buttons = { "all", "tags", "tracks", "albums", "artists" };
		std::string& search;
		std::string	currTerm = "";
		std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> autoCompleteResults;
		std::future<std::unordered_map<std::string, std::vector<std::pair<std::string, int>>>> futureAutoCompleteResults;
		std::future<std::vector<std::pair<std::string, int>>> futureAllResults;
		std::vector<std::pair<std::string, int>> allResults;

		std::chrono::system_clock::time_point lastChange;
		bool changed = false;
	};

}