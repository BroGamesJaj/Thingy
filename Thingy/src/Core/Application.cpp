#include "tpch.h"

#include "Log.h"
#include "Application.h"
#include <iostream>

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