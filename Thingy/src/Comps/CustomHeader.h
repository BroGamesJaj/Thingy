#pragma once
#include "tpch.h"
#include <SDL3\SDL.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include "Core\Managers\MessageManager.h"
#include "Core\Fonts.h"
#include "Core\HelperFunctions.h"

namespace Thingy {
	class CustomHeader {
	public:
		CustomHeader(std::unique_ptr<MessageManager>& messageManager, SDL_Window* window, std::string& searchField);

		void OnRender();

		void AutoComplete();



	private:
		std::unique_ptr<MessageManager>& m_MessageManager;
		SDL_Window* m_Window;
		std::string& search;
		bool autoCompleteOn = false;
	};

}