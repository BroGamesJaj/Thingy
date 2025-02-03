#pragma once
#include "tpch.h"

#include <SDL3\SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#define SDL_MIXER_HINT_DEBUG_MUSIC_INTERFACES
#include <SDL3/SDL_system.h>

namespace Thingy {

	struct WindowProps {
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Thingy", unsigned int width = 1280, unsigned int height = 720)
			: Title(title), Width(width), Height(height) {
		}
	};

	class SDLRenderer {
	public:
		SDLRenderer(const WindowProps& props);
		~SDLRenderer();
		SDLRenderer(const SDLRenderer&) = delete;
		void operator=(const SDLRenderer&) = delete;


		void Init(const WindowProps& props);
		void CleanUp();


		void SetWindowFlags();
		void SetCustomWindowStyle(SDL_Window* window);
		void CreateSDLWindow(const WindowProps& props);
		void CreateRenderer();

		void OnRender();

		static SDLRenderer* Create(const WindowProps& props = WindowProps());

		SDL_Window* GetWindow() { return window; }
		SDL_Renderer* GetRenderer() { return renderer; }

	private:
		Uint32 windowFlags = 0;
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;

	};
}