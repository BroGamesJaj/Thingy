#include "tpch.h"

#include "Log.h"
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

	static void URLSanitizer(std::string& url) {
		url.erase(std::remove(url.begin(), url.end(), '\\'), url.end());
		url.erase(std::remove_if(url.begin(), url.end(), ::isspace), url.end());
	}

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
		audioManager = std::unique_ptr<Thingy::AudioManager>(new Thingy::AudioManager(musicBuffer));
		networkManager = std::unique_ptr<NetworkManager>(new NetworkManager());
		sceneManager = std::unique_ptr<SceneManager>(new SceneManager());
	}

	void Application::SetupScenes() {
		sceneManager->AddScene(std::shared_ptr<FrontPageScene>(new Thingy::FrontPageScene()));
		sceneManager->AddScene(std::shared_ptr<LoginPageScene>(new Thingy::LoginPageScene()));
		sceneManager->GetScenes();
	}

	void Application::Run() {
		SDL_ShowWindow(renderer->GetWindow());
		SDL_Renderer* sdlRenderer = renderer->GetRenderer();
		SDL_Window* sdlWindow = renderer->GetWindow();

		std::string link;


		std::shared_ptr<PlayerModule> playerModule = std::shared_ptr<PlayerModule>(new Thingy::PlayerModule(audioManager));

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

			//Header
			CustomHeader(windowWidth, windowHeight, Running, *sdlWindow);


			ImGui::Begin("Teszt", nullptr);
			//  https:\/\/prod-1.storage.jamendo.com\/?trackid=1848357&format=mp31&from=app-devsite
			ImGui::InputText("link", &link, 0, ResizeCallback, (void*)&link);
			if (ImGui::Button("send")) {

				T_INFO("send");
				if (link.empty()) break;
				URLSanitizer(link);
				networkManager->DownloadFile(link, musicBuffer);

				audioManager->ChangeMusic();
				audioManager->ResumeMusic();
			}


			ImGui::End();

			try {
				playerModule->OnRender();
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
				Running = true;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(renderer->GetWindow()))
				Running = true;

		}
	}

}