#pragma once


namespace Thingy {
	class Application {
	public:
		Application();
		~Application();

		Application(const Application&) = delete;
		void operator=(const Application&) = delete;

		void Run();
	private:
		bool Running = true;
	};

}