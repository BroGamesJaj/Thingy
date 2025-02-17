#pragma once
#include "tpch.h"
#include <imgui.h>
namespace Thingy {
	struct Fonts {
		static ImFont* base;
		static ImFont* size25;
		static ImFont* size30;
		static ImFont* size35;
		static ImFont* size38;
		static ImFont* size40;
		static ImFont* size45;
		static ImFont* size50;
		static ImFont* size60;
		static ImFont* size70;

		static void LoadFonts();
	};
}
