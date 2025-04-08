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
	ImFont* Fonts::icons20 = nullptr;
	ImFont* Fonts::icons30 = nullptr;

	static const ImWchar full_unicode_range[] = { 0x0020, 0xFFFF };

	void Fonts::LoadFonts() {
		ImGuiIO& io = ImGui::GetIO();

		base = io.Fonts->AddFontFromFileTTF("../assets/fonts/dejavu-sans/DejaVuSans.ttf", 16.0f, nullptr, full_unicode_range);
		size25 = io.Fonts->AddFontFromFileTTF("../assets/fonts/dejavu-sans/DejaVuSans.ttf", 25.0f, nullptr, full_unicode_range);
		size30 = io.Fonts->AddFontFromFileTTF("../assets/fonts/dejavu-sans/DejaVuSans.ttf", 30.0f, nullptr, full_unicode_range);
		size35 = io.Fonts->AddFontFromFileTTF("../assets/fonts/dejavu-sans/DejaVuSans.ttf", 35.0f, nullptr, full_unicode_range);
		size40 = io.Fonts->AddFontFromFileTTF("../assets/fonts/dejavu-sans/DejaVuSans.ttf", 40.0f, nullptr, full_unicode_range);
		size45 = io.Fonts->AddFontFromFileTTF("../assets/fonts/dejavu-sans/DejaVuSans.ttf", 45.0f, nullptr, full_unicode_range);
		size50 = io.Fonts->AddFontFromFileTTF("../assets/fonts/dejavu-sans/DejaVuSans.ttf", 50.0f, nullptr, full_unicode_range);
		size60 = io.Fonts->AddFontFromFileTTF("../assets/fonts/dejavu-sans/DejaVuSans.ttf", 60.0f, nullptr, full_unicode_range);
		size70 = io.Fonts->AddFontFromFileTTF("../assets/fonts/dejavu-sans/DejaVuSans.ttf", 70.0f, nullptr, full_unicode_range);
		static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA };
		ImFontConfig config;
		config.PixelSnapH = true;
		icons20 = io.Fonts->AddFontFromFileTTF("../assets/fonts/fa-solid-900.ttf", 20.0f, &config, icon_ranges);
		icons30 = io.Fonts->AddFontFromFileTTF("../assets/fonts/fa-solid-900.ttf", 30.0f, &config, icon_ranges);

		io.Fonts->Build();

		io.FontDefault = base;
	
	}

}