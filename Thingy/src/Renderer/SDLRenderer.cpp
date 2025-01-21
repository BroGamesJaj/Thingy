#include "tpch.h"
#include "SDLRenderer.h"
namespace Thingy {

	SDLRenderer::SDLRenderer() {
	}

	SDLRenderer::~SDLRenderer() {

	}

	void SDLRenderer::Init() {
		if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
			printf("Error: SDL_Init(): %s\n", SDL_GetError());
		}
		SetWindowFlags();
		CreateSDLWindow();
		CreateRenderer();
	}

	void SDLRenderer::CleanUp() {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void SDLRenderer::SetWindowFlags() {
		windowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
	}
	void SDLRenderer::CreateSDLWindow() {
		SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL3+SDL_Renderer example", 1280, 720, windowFlags);
		if (window == nullptr)
		{
			printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		}
	}
	void SDLRenderer::CreateRenderer() {
		SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
		SDL_SetRenderVSync(renderer, 1);
		if (renderer == nullptr)
		{
			SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
		}
	}
	void SDLRenderer::EventLoop(SDL_Event& event) {
		
	}
	void SDLRenderer::OnRender() {

	}
}
