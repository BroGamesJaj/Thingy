#include "tpch.h"
#include "Core/Application.h"
#include <iostream>

#ifdef T_PLATFORM_WINDOWS

	int main(int argc, char** argv) {

		Thingy::Application app{};
		
		app.Run();	

	}

#else
#error Windows support only!
#endif
	