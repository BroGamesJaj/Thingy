#pragma once
#include "tpch.h"
#include <SDL3\SDL.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include "Fonts.h"
#include "HelperFunctions.h"

namespace Thingy {
	void CustomHeader(float& windowWidth, float& windowHeight, bool& done, SDL_Window& window, bool& fullscreen, bool& fullscreenChanged, std::string& search);
}