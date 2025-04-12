#include "tpch.h"

#include "Core/Log.h"
#include "Core/Application.h"
#include "Core/Managers/AudioManager.h"
#include "Core/Managers/NetworkManager.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <stdio.h>
#include <SDL3\SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#define SDL_MIXER_HINT_DEBUG_MUSIC_INTERFACES
#include <SDL3/SDL_system.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <fstream>
#include <curl\curl.h>
#include <curl\easy.h>
#include <nlohmann\json.hpp>
#include "Modules\PlayerModule.h"
#include "Modules\PopularsModule.h"

using json = nlohmann::json;

#ifdef T_PLATFORM_WINDOWS
#ifndef NDEBUG
int main(int argc, char* argv[]) {

	Thingy::Log::Init();
	Thingy::Application app{};
	T_INFO("Thingy started");
	app.Run();
	return 0;
}
#else
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Thingy::Log::Init();
	Thingy::Application app{};
	T_INFO("Thingy started");
	app.Run();
	return 0;
}
#endif
#else
#error Windows support only!
#endif
