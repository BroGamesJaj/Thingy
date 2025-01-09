#include "tpch.h"

#include "Log.h"
#include "Application.h"
#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

namespace Thingy {
	Application::Application() {

	}

	Application::~Application() {

	}

	void Application::Run() {
		T_TRACE("hi");
		int a = 5;
		T_TRACE("{0}", a);
		while (Running) {

		}
	}
}