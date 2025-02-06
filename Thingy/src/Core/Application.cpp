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
	

	void CustomHeader(float& windowWidth, float& windowHeight, bool& done, SDL_Window& window) {

		ImGui::SetNextWindowSize({ windowWidth, windowHeight });
		ImGui::SetNextWindowPos({ 0.0f, 0.0f });

		ImGui::Begin("Custom Header", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGui::GetCurrentWindow()->DC.LayoutType = ImGuiLayoutType_Horizontal;
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 15 - 40 - 5 - 40 - 5 - 40);
		if(ImGui::Button("_", { 40.0f, 30.0f }))
			SDL_MinimizeWindow(&window);
			
		//ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 10 - 40 - 5 - 40); // 10 padding right, width of X button, padding between buttons, width of button
		if (ImGui::Button("[]", { 40.0f, 30.0f })) {
			fullscreen = !fullscreen;
			fullscreenChanged = true;
		}
		//ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 10 - 40); // 10 padding right, width of button
		if(ImGui::Button("X", { 40.0f, 30.0f }))
			done = false;
		
		ImGui::GetCurrentWindow()->DC.LayoutType = ImGuiLayoutType_Vertical;
		ImGui::End();
	}

	static int ResizeCallback(ImGuiInputTextCallbackData* data)
	{
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			ImVector<char>* my_str = (ImVector<char>*)data->UserData;
			IM_ASSERT(my_str->begin() == data->Buf);
			my_str->resize(data->BufSize); // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
			data->Buf = my_str->begin();
		}
		return 0;
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

		SDL_ShowWindow(sdlWindow);
		bool first = true;
		while (Running) {

			EventLoop();
			if (audioManager->IsMusicLoaded())
				audioManager->UpdateTrackPos();

			if (fullscreenChanged) {
				SDL_SetWindowFullscreen(sdlWindow, fullscreen);
				SDL_SetWindowBordered(sdlWindow, false);
				fullscreenChanged = false;
			}
			ImGui_ImplSDLRenderer3_NewFrame();
			ImGui_ImplSDL3_NewFrame();
			ImGui::NewFrame();
			
			//convenience variables
			int winW = 0;
			int winH = 0;
			SDL_GetWindowSizeInPixels(sdlWindow, &winW, &winH);
			ImGuiIO& io = ImGui::GetIO(); (void)io;
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
			CustomHeader(windowWidth, windowHeight, Running, *sdlWindow);

			sceneManager->GetActiveScene()->OnUpdate();
			sceneManager->GetActiveScene()->OnRender();
			
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