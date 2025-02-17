#pragma once
#include "tpch.h"
#include <imgui.h>

namespace Thingy {
	void LimitedTextWrap(const char* text, float maxWidth, int maxLines);

	static int ResizeCallback(ImGuiInputTextCallbackData* data);
}
