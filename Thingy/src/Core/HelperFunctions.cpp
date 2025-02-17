#include "tpch.h"
#include "HelperFunctions.h"

namespace Thingy {
	void LimitedTextWrap(const char* text, float maxWidth, int maxLines) {
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();

		float lineHeight = ImGui::GetTextLineHeight();
		float maxHeight = lineHeight * maxLines;

		ImGui::PushClipRect(
			cursorPos,
			ImVec2(cursorPos.x + maxWidth, cursorPos.y + maxHeight),
			true
		);

		ImGui::PushTextWrapPos(cursorPos.x + maxWidth -10);
		ImGui::Text("%s", text);
		ImGui::PopTextWrapPos();

		ImGui::PopClipRect();

		ImGui::Dummy(ImVec2(0, maxHeight));
	}

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