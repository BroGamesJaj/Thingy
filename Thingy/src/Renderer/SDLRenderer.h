#pragma once

#include <SDL3\SDL.h>
namespace Thingy {

	class SDLRenderer {
	public:
		SDLRenderer();
		~SDLRenderer();
		SDLRenderer(const SDLRenderer&) = delete;
		void operator=(const SDLRenderer&) = delete;


		void Init();
		void CleanUp();


		void SetWindowFlags();
		void CreateSDLWindow();
		void CreateRenderer();

		void EventLoop(SDL_Event& event);
		void OnRender();

	private:
		Uint32 windowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
		SDL_Window* window;
		SDL_Renderer* renderer;
	};
}