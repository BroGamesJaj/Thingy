#include "tpch.h"

#include "Log.h"
#include "Application.h"
#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

namespace Thingy {
	Application::Application() {
		//renderer->Init();
	}

	Application::~Application() {
		//renderer->CleanUp();
	}

	void Application::Run() {
		while (Running) {

		}
	}
}