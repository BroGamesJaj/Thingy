#include "tpch.h"
#include "Application.h"
#include <iostream>
namespace Thingy {
	Application::Application() {

	}

	Application::~Application() {

	}

	void Application::Run() {

		while (Running) {
			std::cout << "hi" << std::endl;
		}
	}
}