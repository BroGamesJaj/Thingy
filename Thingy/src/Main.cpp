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
using json = nlohmann::json;

#include "Modules\PlayerModule.h"
#include "Modules\PopularsModule.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
static const char* windowNames[3] = { "Left Window", "Center Window", "Right Window" };
//static Mix_Music* music = NULL;
#ifdef T_PLATFORM_WINDOWS

//std::vector<char> musicBuffer;




void UpdateDockingLayout() {

	ImGuiID dockspace_id = ImGui::GetID("DockSpace");
	ImGui::DockBuilderRemoveNode(dockspace_id);
	ImGui::DockBuilderAddNode(dockspace_id);

	ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
	ImGui::DockBuilderSetNodeSize(dockspace_id, viewport_size);

	ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.30f, nullptr, &dockspace_id);
	
	ImGuiID dockCenter = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.5714f, nullptr, &dockspace_id);
	ImGuiID dockRight2 = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.4286f, nullptr, &dockspace_id);

	ImGui::DockBuilderDockWindow(windowNames[0], dockLeft);
	ImGui::DockBuilderDockWindow(windowNames[1], dockCenter);
	ImGui::DockBuilderDockWindow(windowNames[2], dockRight2);

	ImGui::DockBuilderFinish(dockspace_id);
}

void LayoutChange(int dragged, ImVec2 currentPos, bool& changed) {
	T_TRACE("dragged: {0}", dragged);
	T_TRACE("{0} {1} {2}", windowNames[0], windowNames[1], windowNames[2]);
	for (size_t i = 0; i < 3; i++) {
		
		if (windowNames[i] != windowNames[dragged]) {
			ImVec2 winSize = ImGui::FindWindowByName(windowNames[i])->Size;
			ImVec2 winPos = ImGui::FindWindowByName(windowNames[i])->Pos;
			if (currentPos.x > winPos.x && currentPos.x < winPos.x + winSize.x && currentPos.y > winPos.y && currentPos.y < winPos.y + winSize.y) {
				std::swap(windowNames[dragged], windowNames[i]);
				T_TRACE("after swap: {0} {1} {2}", windowNames[0], windowNames[1], windowNames[2]);
				changed = true;
				return;
			}
		}
	}
	changed = false;
}

void AlignForWidth(float width, float alignment = 0.5f)
{
	ImGuiStyle& style = ImGui::GetStyle();
	float avail = ImGui::GetContentRegionAvail().x;
	float off = (avail - width) * alignment;
	if (off > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}


void SetCustomWindowStyle(SDL_Window* window) {
	HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
	if (hwnd) {
		LONG style = GetWindowLong(hwnd, GWL_STYLE);
		style &= ~(WS_CAPTION | WS_THICKFRAME);
		style |= WS_POPUP | WS_EX_CLIENTEDGE | WS_SYSMENU;
		SetWindowLong(hwnd, GWL_STYLE, style);
	
		SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}
}


SDL_HitTestResult window_hit_test(SDL_Window* win, const SDL_Point* pos, void*) {
	
	int w, h;
	SDL_GetWindowSize(win, &w, &h);

	const int EDGE_SIZE = 5;
	// Corners
	if (pos->x < EDGE_SIZE && pos->y < EDGE_SIZE) return SDL_HITTEST_RESIZE_TOPLEFT;
	if (pos->x > w - EDGE_SIZE && pos->y < EDGE_SIZE) return SDL_HITTEST_RESIZE_TOPRIGHT;
	if (pos->x < EDGE_SIZE && pos->y > h - EDGE_SIZE) return SDL_HITTEST_RESIZE_BOTTOMLEFT;
	if (pos->x > w - EDGE_SIZE && pos->y > h - EDGE_SIZE) return SDL_HITTEST_RESIZE_BOTTOMRIGHT;

	// Edges
	if (pos->y < EDGE_SIZE) return SDL_HITTEST_RESIZE_TOP;
	if (pos->y > h - EDGE_SIZE) return SDL_HITTEST_RESIZE_BOTTOM;
	if (pos->x < EDGE_SIZE) return SDL_HITTEST_RESIZE_LEFT;
	if (pos->x > w - EDGE_SIZE) return SDL_HITTEST_RESIZE_RIGHT;

	if (pos->y < 50 && pos->y >= EDGE_SIZE && pos->x < w - 145) return SDL_HITTEST_DRAGGABLE;
	return SDL_HITTEST_NORMAL;
}



int main(int argc, char* argv[]) {

	Thingy::Log::Init();
	Thingy::Application app{};
	T_INFO("Thingy started");
	app.Run();
	return 0;
}

#else
#error Windows support only!
#endif
