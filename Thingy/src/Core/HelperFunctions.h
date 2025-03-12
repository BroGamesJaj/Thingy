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

	std::string SecondsToTimeString(int seconds);

	bool OpenFileExplorer(std::string& filePath);

	std::string GetFileExtension(const std::string& filePath);

	bool IsImageFile(const std::string& filePath);

	void CircleImage(ImTextureID user_texture_id, float diameter, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
}
