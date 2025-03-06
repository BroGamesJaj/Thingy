#pragma once
#include "tpch.h"
#include <SDL3\SDL.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include "Core\Managers\MessageManager.h"
#include "Core\Managers\NetworkManager.h"
#include "Core\Fonts.h"
#include "Core\HelperFunctions.h"

namespace Thingy {
	class CustomHeader {
	public:
		CustomHeader(std::unique_ptr<MessageManager>& messageManager, std::unique_ptr<NetworkManager>& netwokrManager, SDL_Window* window, std::string& searchField);

		void OnRender();

		void AutoComplete();




	private:
		std::vector<std::pair<std::string, int>> AllTermResults();

		std::unique_ptr<MessageManager>& m_MessageManager;
		std::unique_ptr<NetworkManager>& m_NetworkManager;
		SDL_Window* m_Window;

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