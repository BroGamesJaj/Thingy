#pragma once
#include "tpch.h"
#include "Renderer\SDLRenderer.h"

namespace Thingy {
	class Application {
	public:
		Application();
		~Application();

		Application(const Application&) = delete;
		void operator=(const Application&) = delete;

		void Run();

		//helper functions

	private:
		bool Running = true;
		std::unique_ptr<SDLRenderer> renderer;
	};

}