#pragma once
#include "tpch.h"
#include <imgui.h>

namespace Thingy {
	void LimitedTextWrap(const char* text, float maxWidth, int maxLines);
	static int ResizeCallback(ImGuiInputTextCallbackData* data) {
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
			ImVector<char>* my_str = (ImVector<char>*)data->UserData;
			IM_ASSERT(my_str->begin() == data->Buf);
			my_str->resize(data->BufSize); // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
			data->Buf = my_str->begin();
		}
		return 0;
	}
}
