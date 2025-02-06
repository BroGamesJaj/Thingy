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


SDL_Cursor* CreateCustomCursor(const char* imagePath, int newWidth = 32, int newHeight = 32) {
	// Load BMP image
	SDL_Surface* originalSurface = SDL_LoadBMP(imagePath);
	if (!originalSurface) {
		SDL_Log("Failed to load cursor image: %s", SDL_GetError());
		return nullptr;
	}

	// Convert to RGBA32 format for compatibility
	SDL_Surface* convertedSurface = SDL_ConvertSurface(originalSurface, SDL_PIXELFORMAT_RGBA32);
	SDL_DestroySurface(originalSurface); // Free original surface
	if (!convertedSurface) {
		SDL_Log("Failed to convert surface format: %s", SDL_GetError());
		return nullptr;
	}

	// Create resized surface with matching format
	SDL_Surface* resizedSurface = SDL_CreateSurface(newWidth, newHeight, convertedSurface->format);
	if (!resizedSurface) {
		SDL_Log("Failed to create resized surface: %s", SDL_GetError());
		SDL_DestroySurface(convertedSurface);
		return nullptr;
	}

	// Scale image
	SDL_Rect srcRect = { 0, 0, convertedSurface->w, convertedSurface->h };
	SDL_Rect dstRect = { 0, 0, newWidth, newHeight };
	if (!SDL_BlitSurfaceScaled(convertedSurface, &srcRect, resizedSurface, &dstRect, SDL_SCALEMODE_LINEAR)) {
		SDL_Log("Failed to scale image: %s", SDL_GetError());
		SDL_DestroySurface(convertedSurface);
		SDL_DestroySurface(resizedSurface);
		return nullptr;
	}

	// Create cursor from resized surface
	SDL_Cursor* customCursor = SDL_CreateColorCursor(resizedSurface, 0, 0);

	// Clean up
	SDL_DestroySurface(convertedSurface);
	SDL_DestroySurface(resizedSurface);

	return customCursor;
}

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
	/*
	
	// Setup SDL
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
	{
		printf("Error: SDL_Init(): %s\n", SDL_GetError());
		return -1;
	}
	//std::string jsonData = GetRequest("https://api.jamendo.com/v3.0/tracks/?client_id=8b1de417&format=jsonpretty&limit=5&fuzzytags=groove+rock&speed=high+veryhigh&include=musicinfo&groupby=artist_id");
	//json parsedJsonData = json::parse(jsonData);
	SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
	std::unique_ptr<Thingy::NetworkManager> networkManager;

	std::unique_ptr<Thingy::AudioManager> audioManager = std::unique_ptr<Thingy::AudioManager>(new Thingy::AudioManager(musicBuffer));
	
	networkManager->DownloadFile("https:\/\/prod-1.storage.jamendo.com\/?trackid=1848357&format=mp31&from=app-devsite", musicBuffer);

	audioManager->ChangeMusic();
	audioManager->ResumeMusic();

	// Create window with SDL_Renderer graphics context
	Uint32 window_flags =  SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
	SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL3+SDL_Renderer example", SCREEN_WIDTH, SCREEN_HEIGHT, window_flags);
	if (window == nullptr)
	{
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return -1;
	}
	SetCustomWindowStyle(window);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
	SDL_SetRenderVSync(renderer, 1);
	if (renderer == nullptr) {
		SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
		return -1;
	}
	SDL_ShowWindow(window);
	if (!SDL_SetWindowHitTest(window, window_hit_test, nullptr)) {
		SDL_Log("Failed to set hit test: %s", SDL_GetError());
	}
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	
	
	SDL_Texture* my_texture;
	int my_image_width, my_image_height;
	bool ret = LoadTextureFromFile("../assets/images/adatmodel.jpg", renderer, &my_texture, &my_image_width, &my_image_height);
	IM_ASSERT(ret);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);
	
	io.ConfigDebugIniSettings = true;
	io.MouseDragThreshold = 4.0f;
	bool draggingWindow[3] = {false, false, false};
	ImVec2 dragStartPos;
	bool changed = false;
	ImGuiStyle& style = ImGui::GetStyle();	
	style.FramePadding = {10.0f, 10.0f};
	//custom cursor
	int cursorWidth = GetSystemMetrics(SM_CXCURSOR);
	int cursorHeight = GetSystemMetrics(SM_CYCURSOR);
	std::map<std::string, SDL_Cursor*> customCursors{ {"openHand",CreateCustomCursor("../assets/cursors/openHand.bmp", cursorWidth / 2, cursorHeight / 2)}, {"closedHand", CreateCustomCursor("../assets/cursors/closedHand.bmp", cursorWidth / 2, cursorHeight / 2)}};
	
	T_INFO("{0}w", cursorWidth);
	T_INFO("{0}h", cursorHeight);
	std::shared_ptr<Thingy::PopularsModule> populars = std::shared_ptr<Thingy::PopularsModule>(new Thingy::PopularsModule);
	// Main loop
	bool done = false;
	while (!done)
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT)
				done = true;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
				done = true;
			
		}
		if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
		{
			SDL_Delay(10);
			continue;
		}
		audioManager->UpdateTrackPos();
		SDL_SetCursor(NULL);
		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
		int winW = 0;
		int winH = 0;
		SDL_GetWindowSizeInPixels(window, &winW, &winH);
		float windowWidth = static_cast<float>(winW);
		float windowHeight = static_cast<float>(winH);

		
		ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		main_viewport->WorkPos = ImVec2(10,50);
		main_viewport->WorkSize = ImVec2( windowWidth-20, windowHeight-55);
		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoResizeY | ImGuiDockNodeFlags_AutoHideTabBar | ImGuiDockNodeFlags_NoTabBar;
		ImGui::DockSpaceOverViewport(dockspace_id, main_viewport, dockSpaceFlags);
		
		
		static bool dock_initialized = false;

		CustomHeader(windowWidth, windowHeight, done);
		
		std::shared_ptr<Thingy::PlayerModule> module = std::shared_ptr<Thingy::PlayerModule>(new Thingy::PlayerModule(audioManager));
		// Debug window
		//ImGui::ShowDebugLogWindow();
		bool isHoveringAnyBar = false;
		bool isDraggingAnyBar = false;
		for (int i = 0; i < 3; i++){
			static float f = 0.0f;
			static int counter = 0;
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
			ImGui::Begin(windowNames[i], nullptr, flags);
			float window_width = ImGui::GetWindowWidth();
			ImVec2 bar_size = ImVec2(window_width-20, 30);
			if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left) && !ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
				draggingWindow[i] = false;
			}
			ImGui::InvisibleButton("DragBar", bar_size);
			if (ImGui::IsItemHovered() && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
				isHoveringAnyBar = true;
			}
			if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				
				if (!draggingWindow[i])
				{
					draggingWindow[i] = true;
					dragStartPos = ImGui::GetMousePos();
					isDraggingAnyBar = true;
				}
			}
			ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255), 0.0f, 0, 5.0f);
			ImGui::Text("window size: (%.1f, %.1f)", ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
			ImGui::Button("Stop music", { 100.0f, 50.0f });
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
				Mix_PauseMusic();
			}
			ImGui::Button("Resume music", { 100.0f, 50.0f });
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
				Mix_ResumeMusic();
			}
			if (draggingWindow[i] && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
				ImVec2 currentPos = ImGui::GetMousePos();
				ImVec2 windowPos = ImGui::GetWindowPos();
				draggingWindow[i] = false;
				T_ERROR("changed {0}", changed);
				LayoutChange(i, currentPos, changed);
				T_ERROR("changed after {0}", changed);
			}
			if (draggingWindow[i] && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
				isDraggingAnyBar = true;
				T_TRACE("Mouse dragging {0}, {1}",windowNames[i],i);
				ImVec2 currentPos = ImGui::GetMousePos();
				ImVec2 windowPos = ImGui::GetWindowPos();
				//if (currentPos.x > windowPos.x+ImGui::GetWindowWidth()+10 && i != 2) {
				//	LayoutChange(i, true);
				//	draggingWindow[i] = false;
				//	changed = true;
				//}
				//if (currentPos.x < windowPos.x-10 && i != 0) {
				//	LayoutChange(i, false);
				//	draggingWindow[i] = false;
				//	changed = true;
				//}
			}
			
			ImGui::Text("This is %s", windowNames[i]);
			ImGuiStyle& style = ImGui::GetStyle();
			float width = 0.0f;
			width += ImGui::CalcTextSize("Hello").x;
			width += style.ItemSpacing.x;
			width += 150.0f;
			width += style.ItemSpacing.x;
			width += ImGui::CalcTextSize("World!").x;
			AlignForWidth(width);

			ImGui::Button("Hello");
			ImGui::SameLine();
			ImGui::Button("Fixed");
			ImGui::SameLine();
			ImGui::Button("World");

			ImGui::SliderInt("time", &audioManager->GetCurrentTrackPos(), 0, audioManager->GetCurrentTrack().duration);
			if (ImGui::IsItemEdited()) {
				audioManager->ChangeMusicPos();
			}
			ImGui::SliderInt("Volume slider", &audioManager->GetVolume(), 0, MIX_MAX_VOLUME);
			if (ImGui::IsItemEdited()) {
				audioManager->ChangeVolume();
			}

			if (ImGui::Button("Button"))         
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);
			//ImGui::Text("pointer = %p", my_texture);
			//ImGui::Text("size = %d x %d", my_image_width, my_image_height);
			//ImGui::Image((ImTextureID)(intptr_t)my_texture, ImVec2((float)my_image_width, (float)my_image_height));
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}
		
		if (!dock_initialized || changed) {
			UpdateDockingLayout();
			dock_initialized = true;
			changed = false;
		}
		
		if (isDraggingAnyBar) {
			if (!SDL_SetCursor(customCursors["closedHand"])) {
				SDL_GetError();
			}
		}
		else if (isHoveringAnyBar) {
			if (!SDL_SetCursor(customCursors["openHand"])) {
				SDL_GetError();
			}
		}
		else {
			SDL_SetCursor(SDL_GetDefaultCursor());
		}
		
		for (int i = 0; i < 3; i++) {
			if (draggingWindow[i]) {


				static float f = 0.0f;
				static int counter = 0;
				ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
				ImGui::BeginDisabled();

				ImGui::Begin("floater", nullptr, flags);
				//ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.3f);
				float window_width = ImGui::GetWindowWidth();
				ImVec2 bar_size = ImVec2(window_width - 20, 30);
				
				ImGui::InvisibleButton("DragBar", bar_size);
				
				
				ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255), 0.0f, 0, 5.0f);
				ImGui::Text("window size: (%.1f, %.1f)", ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
				ImGui::Button("Stop music", { 100.0f, 50.0f });
				
				ImGui::Button("Resume music", { 100.0f, 50.0f });
				

				ImGui::Text("This is %s", windowNames[i]);

				ImGui::SliderInt("time", &audioManager->GetCurrentTrackPos(), 0, audioManager->GetCurrentTrack().duration);
				
				ImGui::SliderInt("Volume slider", &audioManager->GetVolume(), 0, MIX_MAX_VOLUME);
				

				if (ImGui::Button("Button"))
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);
				//ImGui::Text("pointer = %p", my_texture);
				//ImGui::Text("size = %d x %d", my_image_width, my_image_height);
				//ImGui::Image((ImTextureID)(intptr_t)my_texture, ImVec2((float)my_image_width, (float)my_image_height));
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
				//ImGui::PopStyleVar();

				ImGui::SetWindowPos({ ImGui::GetMousePos().x - (ImGui::FindWindowByName(windowNames[i])->Size.x / 2), ImGui::GetMousePos().y + 5 });
				ImGui::SetWindowSize(ImGui::FindWindowByName(windowNames[i])->Size);
				ImGui::End();
				ImGui::EndDisabled();
			}
		}

		try {
			module->OnRender();
			//populars->OnRender();

		} catch (const std::exception& e) {
			// Handle standard exceptions
			std::cerr << "An error occurred: " << e.what() << std::endl;
		} catch (...) {
			// Handle unknown exceptions
			std::cerr << "An unknown error occurred." << std::endl;
		}
		// Rendering
		ImGui::Render();
		SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		SDL_RenderClear(renderer);
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
		SDL_RenderPresent(renderer);
	}

	for (auto& pair : customCursors) {
		SDL_DestroyCursor(pair.second);
	}

	// Cleanup
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	*/

	app.Run();
	return 0;
}

#else
#error Windows support only!
#endif
