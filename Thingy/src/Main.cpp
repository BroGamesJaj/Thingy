#include "tpch.h"
#include "Core/Application.h"
#include <iostream>

#ifdef T_PLATFORM_WINDOWS

	int main(int argc, char** argv) {

		Thingy::Application app{};
		T_TRACE()
		app.Run();	

	}

#else
#error Windows support only!
#endif
	