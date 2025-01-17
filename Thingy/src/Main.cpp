#include "tpch.h"

#include "Core/Log.h"
#include "Core/Application.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <stdio.h>
#include <SDL3\SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <fstream>
#include <curl\curl.h>
#include <curl\easy.h>



const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
static const char* windowNames[3] = { "Left Window", "Center Window", "Right Window" };
static const char* identifier[3] = { "Left Window", "Center Window", "Right Window" };
static int audioOpen = 0;
static Mix_Music* music = NULL;
static int next_track = 0;

#ifdef T_PLATFORM_WINDOWS

void UpdateDockingLayout() {

	ImGuiID dockspace_id = ImGui::GetID("DockSpace");

	// Remove any existing dock nodes and create a new dockspace
	ImGui::DockBuilderRemoveNode(dockspace_id);
	ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoResize);
	ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
	ImGui::DockBuilderSetNodeSize(dockspace_id, viewport_size);
	// First, split the dockspace into two regions: left (30%) and right (70%)
	ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.30f, nullptr, &dockspace_id);
	ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.70f, nullptr, &dockspace_id);

	// Now, split the remaining right part (70%) into two sections: center (40%) and right (30%)
	ImGuiID dockCenter = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.5714f, nullptr, &dockspace_id);
	ImGuiID dockRight2 = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.4286f, nullptr, &dockspace_id);

	// Dock windows into the appropriate regions
	ImGui::DockBuilderDockWindow(windowNames[0], dockLeft);
	ImGui::DockBuilderDockWindow(windowNames[1], dockCenter);
	ImGui::DockBuilderDockWindow(windowNames[2], dockRight2);

	// Finalize the dock builder setup
	ImGui::DockBuilderFinish(dockspace_id);
}

//drag_dir false = left, true = right
void LayoutChange(int dragged, bool drag_dir) {
	T_TRACE("dragged: {0}", dragged);
	// 0 + 1 = 1
	//
	//
	int target = dragged + (drag_dir ? 1 : -1);
	T_TRACE("target: {0}", target);
	if (target < 0 || target > 2) return;
	T_TRACE("{0} {1} {2}", windowNames[0], windowNames[1], windowNames[2]);
	std::swap(windowNames[dragged], windowNames[target]);
	T_TRACE("after swap: {0} {1} {2}", windowNames[0], windowNames[1], windowNames[2]);
	UpdateDockingLayout();
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t totalSize = size * nmemb;
	std::vector<char>* buffer = static_cast<std::vector<char>*>(userp);
	buffer->insert(buffer->end(), static_cast<char*>(contents), static_cast<char*>(contents) + totalSize);
	return totalSize;
}

bool DownloadFile(const std::string& url, std::vector<char>& buffer) {
	CURL* curl = curl_easy_init();
	if (!curl) return false;

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	return (res == CURLE_OK);
}

Mix_Music* LoadMusicFromMemory(const std::vector<char>& buffer) {
	SDL_IOStream* ioStream = SDL_IOFromConstMem(buffer.data(), buffer.size());
	if (!ioStream) {
		SDL_Log("Failed to create RWops: %s\n", SDL_GetError());
		return nullptr;
	}

	Mix_Music* music = Mix_LoadMUS_IO(ioStream, 1);
	if (!music) {
		SDL_Log("Failed to load music: %s\n", SDL_GetError());
	}
	return music;
}


int main(int argc, char* argv[])
{

	Thingy::Log::Init();
	Thingy::Application app{};
	T_INFO("Thingy started");
	// Setup SDL
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
	{
		printf("Error: SDL_Init(): %s\n", SDL_GetError());
		return -1;
	}

	int looping = 0;
	bool interactive = false;
	bool use_io = false;
	int i;
	const char* typ;
	int loopEnd = 0;
	int loopStart, loopLength, currentPosition;
	int audioVolume = MIX_MAX_VOLUME;
	SDL_AudioSpec spec;
	spec.freq = MIX_DEFAULT_FREQUENCY;
	spec.format = MIX_DEFAULT_FORMAT;
	spec.channels = MIX_DEFAULT_CHANNELS;

	if (!Mix_OpenAudio(0, &spec)) {
		SDL_Log("Couldn't open audio: %s\n", SDL_GetError());
	} else {
		Mix_QuerySpec(&spec.freq, &spec.format, &spec.channels);
		SDL_Log("Opened audio at %d Hz %d bit%s %s audio buffer\n", spec.freq,
			(spec.format & 0xFF),
			(SDL_AUDIO_ISFLOAT(spec.format) ? " (float)" : ""),
			(spec.channels > 2) ? "surround" : (spec.channels > 1) ? "stereo" : "mono");
	}
	audioOpen = 1;
	Mix_VolumeMusic(audioVolume);

	std::vector<char> musicBuffer;
	std::string musicURL = "https:\/\/prod-1.storage.jamendo.com\/?trackid=1848357&format=mp31&from=app-devsite";

	if (DownloadFile(musicURL, musicBuffer)) {
		music = LoadMusicFromMemory(musicBuffer);
		if (music) {
			if (Mix_PlayMusic(music, -1) == -1) {
				SDL_Log("Failed to play music: %s\n", SDL_GetError());
			}
			loopEnd = Mix_MusicDuration(music);
		}
	}
	else {
		SDL_Log("Failed to download music from URL.\n");
	}

	// Create window with SDL_Renderer graphics context
	Uint32 window_flags =  SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
	SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL3+SDL_Renderer example", SCREEN_WIDTH, SCREEN_HEIGHT, window_flags);
	if (window == nullptr)
	{
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return -1;
	}
	SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
	SDL_SetRenderVSync(renderer, 1);
	if (renderer == nullptr)
	{
		SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
		return -1;
	}
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	
	SDL_ShowWindow(window);

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



	bool draggingWindow[3] = {false, false, false};
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

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		ImGui::DockSpaceOverViewport(dockspace_id, main_viewport, dockspace_flags);

		if (Mix_PlayingMusic() || Mix_PausedMusic()) {
			currentPosition = Mix_GetMusicPosition(music);
		}
		// Create the docking layout only once
		static bool dock_initialized = false;
		if (!dock_initialized)
		{
			UpdateDockingLayout();
			dock_initialized = true;
		}
		
		for (int i = 0; i < 3; i++){
			static float f = 0.0f;
			static int counter = 0;
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
			ImGui::Begin(windowNames[i], nullptr, flags);
			
			if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && ImGui::IsItemHovered()) {
				draggingWindow[i] = true;
			}
			ImGui::Text("window size: (%.1f, %.1f)", ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
			ImGui::Button("Stop music", { 100.0f, 50.0f });
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
				Mix_PauseMusic();
			}
			ImGui::Button("Resume music", { 100.0f, 50.0f });
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
				Mix_ResumeMusic();
			}
			if (draggingWindow[i]) {
				T_TRACE("Mouse dragging {0}, {1}",windowNames[i],i);
				ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
				ImGui::Text("mouse delta: (%.1f, %.1f)", drag_delta.x, drag_delta.y);

				if (drag_delta.x > 100 && i != 2) {
					LayoutChange(i, true);
					draggingWindow[i] = false;
				}
				if (drag_delta.x < -100 && i != 0) {
					LayoutChange(i, false);
					draggingWindow[i] = false;
				}
			}
			if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
				draggingWindow[i] = false;
			}
			ImGui::Text("This is %s",identifier[i]);

			ImGui::SliderInt("time", &currentPosition, 0, loopEnd);
			if (ImGui::IsItemEdited()) {
				Mix_SetMusicPosition(currentPosition);
			}
			ImGui::SliderInt("Volume slider", &audioVolume, 0, MIX_MAX_VOLUME);
			if (ImGui::IsItemEdited()) {
				Mix_VolumeMusic(audioVolume);
			}

			if (ImGui::Button("Button"))         
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}


		// Rendering
		ImGui::Render();
		SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_RenderClear(renderer);
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
		SDL_RenderPresent(renderer);
	}

	// Cleanup
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	Mix_FreeMusic(music);
	music = NULL;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	app.Run();
	return 0;
}

#else
#error Windows support only!
#endif
