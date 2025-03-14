#include "tpch.h"

#include "Application.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <imgui_stdlib.h>

#include "Modules\Modules.h"
#include "Scenes\Scenes.h"

namespace Thingy {

	std::vector<uint8_t> musicBuffer;
	bool fullscreen = false;
	bool fullscreenChanged = false;
	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;
	
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

	Application::Application() {
		begin = std::chrono::steady_clock::now();
		renderer = std::unique_ptr<SDLRenderer>(SDLRenderer::Create());
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui::StyleColorsDark();

		ImGui_ImplSDL3_InitForSDLRenderer(renderer->GetWindow(), renderer->GetRenderer());
		ImGui_ImplSDLRenderer3_Init(renderer->GetRenderer());
		
		SetupManagers();
		StartSubscriptions();
		SetupScenes();
		
		int cursorWidth = GetSystemMetrics(SM_CXCURSOR);
		int cursorHeight = GetSystemMetrics(SM_CYCURSOR);
		customCursors.emplace("openHand", CreateCustomCursor("../assets/cursors/openHand.bmp", cursorWidth / 2, cursorHeight / 2));
		customCursors.emplace("closedHand", CreateCustomCursor("../assets/cursors/closedHand.bmp", cursorWidth / 2, cursorHeight / 2));
		customHeader = std::make_unique<CustomHeader>(*messageManager, *networkManager, *imageManager, *authManager, renderer->GetWindow(), searchTerm);
		//for testing
		//networkManager->DownloadFile("https:\/\/prod-1.storage.jamendo.com\/?trackid=1848357&format=mp31&from=app-devsite", musicBuffer);
		//
		//audioManager->ChangeMusic();
		//audioManager->ResumeMusic();
	}

	Application::~Application() {
		
	}

	void Application::SetupManagers() {
		messageManager = std::make_unique<MessageManager>();
		networkManager = std::make_unique<NetworkManager>(*messageManager);
		imageManager = std::make_unique<ImageManager>(*networkManager, renderer->GetRenderer());
		audioManager = std::make_unique<AudioManager>(musicBuffer, *networkManager, *messageManager);
		sceneManager = std::make_unique<SceneManager>(*messageManager);
		authManager = std::make_unique<AuthManager>(*networkManager, *messageManager);
	}

	void Application::SetupScenes() {
		
		sceneManager->AddScene(std::make_shared<FrontPageScene>(*messageManager));
		sceneManager->AddScene(std::make_shared<LoginScene>(*messageManager));
		sceneManager->AddScene(std::make_shared<ProfileScene>(*messageManager));
		sceneManager->AddScene(std::make_shared<AlbumScene>(*messageManager));
		sceneManager->AddScene(std::make_shared<ArtistScene>(*messageManager));
		sceneManager->GetScenes();
		storedModules.emplace("loginModule", std::make_shared<LoginModule>(*messageManager, *networkManager, *authManager));
		storedModules.emplace("profileModule", std::make_shared<ProfileModule>(*messageManager, *imageManager, *networkManager, *authManager));
		storedModules.emplace("popularsModule", std::make_shared<PopularsModule>(*messageManager, *networkManager, *audioManager, *imageManager, renderer->GetRenderer()));
		storedModules.emplace("albumModule", std::make_shared<AlbumModule>(*messageManager, *audioManager, *imageManager, *networkManager));
		storedModules.emplace("artistModule", std::make_shared<ArtistModule>(*messageManager, *audioManager, *imageManager, *networkManager));
		storedModules.emplace("playerModule", std::make_shared<PlayerModule>(*messageManager, *audioManager, *imageManager, *authManager));
		storedModules.emplace("playlistListModule", std::make_shared<PlaylistListModule>(*messageManager, *imageManager, *networkManager, *authManager));
		

		sceneManager->GetScene("FrontPage")->PushModule(storedModules["playlistListModule"]);
		sceneManager->GetScene("FrontPage")->PushModule(storedModules["popularsModule"]);
		sceneManager->GetScene("FrontPage")->PushModule(storedModules["playerModule"]);
		
		sceneManager->GetScene("LoginScene")->PushModule(storedModules["loginModule"]);

		sceneManager->GetScene("ProfileScene")->PushModule(storedModules["profileModule"]);

		sceneManager->GetScene("AlbumScene")->PushModule(storedModules["playlistListModule"]);
		sceneManager->GetScene("AlbumScene")->PushModule(storedModules["albumModule"]);
		sceneManager->GetScene("AlbumScene")->PushModule(storedModules["playerModule"]);
		
		sceneManager->GetScene("ArtistScene")->PushModule(storedModules["playlistListModule"]);
		sceneManager->GetScene("ArtistScene")->PushModule(storedModules["artistModule"]);
		sceneManager->GetScene("ArtistScene")->PushModule(storedModules["playerModule"]);
		
		messageManager->Publish("homeButton", std::string("FrontPage"));
		sceneManager->GetScenes();

		for (auto& module : storedModules) {
			module.second->SetupSubscriptions();
		}
	}

	void Application::StartSubscriptions() {
		messageManager->Subscribe("closeWindow", "start", [this](const MessageData data) {
			Running = false;
			});
		messageManager->Subscribe("minimize", "start", [this](const MessageData data) {
			SDL_MinimizeWindow(renderer->GetWindow());
			});
		messageManager->Subscribe("changeFullscreen", "start", [this](const MessageData data) {
			T_INFO("fullscreen");
			fullscreen = !fullscreen;
			fullscreenChanged = true;
			});

	}

	void Application::Run() {

		SDL_Renderer* sdlRenderer = renderer->GetRenderer();
		SDL_Window* sdlWindow = renderer->GetWindow();
		Fonts::LoadFonts();

		authManager->RefreshTokens();

		SDL_ShowWindow(sdlWindow);
		bool first = true;
		while (Running) {

			EventLoop();
			if (audioManager->IsMusicLoaded())
				audioManager->UpdateTrackPos();

			if (fullscreenChanged) {
				SDL_SetWindowFullscreen(sdlWindow, fullscreen);
				SDL_SetWindowBordered(sdlWindow, false);
				renderer->ChangeHitTest(!fullscreen);
				fullscreenChanged = false;
			}
			ImGui_ImplSDLRenderer3_NewFrame();
			ImGui_ImplSDL3_NewFrame();
			ImGui::NewFrame();

			//convenience variables
			int winW = 0;
			int winH = 0;
			SDL_GetWindowSizeInPixels(sdlWindow, &winW, &winH);
			ImGuiIO& io = ImGui::GetIO();
			float windowWidth = static_cast<float>(winW);
			float windowHeight = static_cast<float>(winH);
			ImGuiStyle& style = ImGui::GetStyle();
			style.DockingSeparatorSize = 10.0f;
			ImGuiViewport* main_viewport = ImGui::GetMainViewport();
			main_viewport->WorkPos = ImVec2(10, 50);
			main_viewport->WorkSize = ImVec2(windowWidth - 20, windowHeight - 60);
			ImGuiID dockspace_id = ImGui::GetID("DockSpace");
			ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_AutoHideTabBar | ImGuiDockNodeFlags_NoTabBar;
			ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(100, 0.0f));
			ImGui::DockSpaceOverViewport(dockspace_id, main_viewport, dockSpaceFlags);
			ImGui::PopStyleVar();
			if (sceneManager->GetLayoutChanged()) {
				sceneManager->GetActiveScene()->UpdateLayout();
				sceneManager->ResetLayoutChanged();
			}


			//Header

			sceneManager->GetActiveScene()->OnUpdate();
			uint16_t upProps = sceneManager->GetActiveScene()->OnRender();

			customHeader->OnRender();

			if (upProps & BIT(0)) {
				if (!SDL_SetCursor(customCursors["closedHand"])) {
					SDL_GetError();
				}
			} else if (upProps & BIT(2)) {
				if (!SDL_SetCursor(customCursors["openHand"])) {
					SDL_GetError();
				}
			} else if (upProps & BIT(3)) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			} else {
				SDL_SetCursor(SDL_GetDefaultCursor());
			}

			ImGui::Render();
			SDL_SetRenderScale(sdlRenderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
			SDL_SetRenderDrawColor(sdlRenderer, 0, 255, 0, 255);
			SDL_RenderClear(sdlRenderer);
			ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), sdlRenderer);
			SDL_RenderPresent(sdlRenderer);
			if (first) {
				first = false;
				end = std::chrono::steady_clock::now();

				T_INFO("to first frame time:");
				std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[seconds]" << std::endl;
				std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[milliseconds]" << std::endl;
				std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[microseconds]" << std::endl;
			}
			
		}
		ImGui_ImplSDLRenderer3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
		audioManager->CleanUp();
		renderer->CleanUp();

	}

	void Application::EventLoop() {
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT)
				Running = false;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(renderer->GetWindow()))
				Running = false;

		}
	}

}