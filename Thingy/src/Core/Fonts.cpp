#include "tpch.h"
#include "Fonts.h"

namespace Thingy {

	ImFont* Fonts::base = nullptr;
	ImFont* Fonts::size25 = nullptr;
	ImFont* Fonts::size30 = nullptr;
	ImFont* Fonts::size35 = nullptr;
	ImFont* Fonts::size38 = nullptr;
	ImFont* Fonts::size40 = nullptr;
	ImFont* Fonts::size45 = nullptr;
	ImFont* Fonts::size50 = nullptr;
	ImFont* Fonts::size60 = nullptr;
	ImFont* Fonts::size70 = nullptr;

	void Fonts::LoadFonts() {
		ImGuiIO& io = ImGui::GetIO();

		base = io.Fonts->AddFontFromFileTTF("../assets/fonts/Quicksand-Bold.ttf", 16.0f);
		size25 = io.Fonts->AddFontFromFileTTF("../assets/fonts/Quicksand-Bold.ttf", 25.0f);
		size30 = io.Fonts->AddFontFromFileTTF("../assets/fonts/Quicksand-Bold.ttf", 30.0f);
		size35 = io.Fonts->AddFontFromFileTTF("../assets/fonts/Quicksand-Bold.ttf", 35.0f);
		size40 = io.Fonts->AddFontFromFileTTF("../assets/fonts/Quicksand-Bold.ttf", 40.0f);
		size45 = io.Fonts->AddFontFromFileTTF("../assets/fonts/Quicksand-Bold.ttf", 45.0f);
		size50 = io.Fonts->AddFontFromFileTTF("../assets/fonts/Quicksand-Bold.ttf", 50.0f);
		size60 = io.Fonts->AddFontFromFileTTF("../assets/fonts/Quicksand-Bold.ttf", 60.0f);
		size70 = io.Fonts->AddFontFromFileTTF("../assets/fonts/Quicksand-Bold.ttf", 70.0f);

		io.FontDefault = base;
	
	}

}