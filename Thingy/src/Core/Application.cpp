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

		SetupScenes();
		
		int cursorWidth = GetSystemMetrics(SM_CXCURSOR);
		int cursorHeight = GetSystemMetrics(SM_CYCURSOR);
		customCursors.emplace("openHand", CreateCustomCursor("../assets/cursors/openHand.bmp", cursorWidth / 2, cursorHeight / 2));
		customCursors.emplace("closedHand", CreateCustomCursor("../assets/cursors/closedHand.bmp", cursorWidth / 2, cursorHeight / 2));
		//for testing
		//networkManager->DownloadFile("https:\/\/prod-1.storage.jamendo.com\/?trackid=1848357&format=mp31&from=app-devsite", musicBuffer);
		//
		//audioManager->ChangeMusic();
		//audioManager->ResumeMusic();
	}

	Application::~Application() {
		
	}

	void Application::SetupManagers() {
		networkManager = std::unique_ptr<NetworkManager>(new NetworkManager());
		imageManager = std::unique_ptr<ImageManager>(new ImageManager(networkManager, renderer->GetRenderer()));
		audioManager = std::unique_ptr<AudioManager>(new AudioManager(musicBuffer, networkManager));
		messageManager = std::unique_ptr<MessageManager>(new MessageManager());
		sceneManager = std::unique_ptr<SceneManager>(new SceneManager(messageManager));
	}

	void Application::SetupScenes() {
		
		sceneManager->AddScene(std::shared_ptr<FrontPageScene>(new FrontPageScene()));
		sceneManager->AddScene(std::shared_ptr<LoginScene>(new LoginScene()));
		sceneManager->AddScene(std::shared_ptr<AlbumScene>(new AlbumScene()));
		sceneManager->GetScenes();
		storedModules.emplace("popularsModule", std::make_shared<PopularsModule>(messageManager, networkManager, audioManager, imageManager, renderer->GetRenderer()));
		storedModules.emplace("albumModule", std::make_shared<AlbumModule>(messageManager, audioManager, imageManager));
		storedModules.emplace("playerModule", std::make_shared<PlayerModule>(messageManager, audioManager, imageManager));
		sceneManager->GetScene("FrontPage")->PushModule(storedModules["popularsModule"]);
		sceneManager->GetScene("FrontPage")->PushModule(storedModules["playerModule"]);
		sceneManager->GetScene("AlbumScene")->PushModule(storedModules["albumModule"]);
		sceneManager->SetActiveScene("FrontPage");
		sceneManager->GetScenes();

		for (auto& module : storedModules) {
			module.second->SetupSubscriptions();
		}

	}

	void Application::Run() {

		SDL_Renderer* sdlRenderer = renderer->GetRenderer();
		SDL_Window* sdlWindow = renderer->GetWindow();
		Fonts::LoadFonts();

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
			ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoResizeY | ImGuiDockNodeFlags_AutoHideTabBar | ImGuiDockNodeFlags_NoTabBar;
			ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(100, 0.0f));
			ImGui::DockSpaceOverViewport(dockspace_id, main_viewport, dockSpaceFlags);
			ImGui::PopStyleVar();
			if (sceneManager->GetLayoutChanged()) {
				sceneManager->GetActiveScene()->UpdateLayout();
				sceneManager->ResetLayoutChanged();
			}


			//Header
			CustomHeader(windowWidth, windowHeight, Running, *sdlWindow, fullscreen, fullscreenChanged, searchTerm);

			sceneManager->GetActiveScene()->OnUpdate();
			uint16_t upProps = sceneManager->GetActiveScene()->OnRender();

			//ImGui::Begin("Teszt", nullptr);
			//ImGui::InputText("link", &link, 0, ResizeCallback, (void*)&link);
			//if (ImGui::Button("send")) {
			//
			//	T_INFO("send");
			//	if (link.empty()) break;
			//	
			//	networkManager->GetArtist(link);
			//
			//}
			//ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2((float)300, (float)300));
			//
			//ImGui::End();

			try {
				//playerModule->OnRender();
				//populars->OnRender();

			}
			catch (const std::exception& e) {
				std::cerr << "An error occurred: " << e.what() << std::endl;
			}
			catch (...) {
				std::cerr << "An unknown error occurred." << std::endl;
			}
			if (upProps & BIT(0)) {
				if (!SDL_SetCursor(customCursors["closedHand"])) {
					SDL_GetError();
				}
			} else if (upProps & BIT(2)) {
				if (!SDL_SetCursor(customCursors["openHand"])) {
					SDL_GetError();
				}
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