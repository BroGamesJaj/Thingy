#include "tpch.h"
#include "HelperFunctions.h"

namespace Thingy {
	void LimitedTextWrap(const char* text, float maxWidth, int maxLines) {
		/*
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
		*/
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImVec2 cursorLocalPos = ImGui::GetCursorPos(); // Relative to scrolling region

		float lineHeight = ImGui::GetTextLineHeight();
		float maxHeight = lineHeight * maxLines;

		ImGui::PushClipRect(
			ImVec2(cursorPos.x, cursorPos.y),
			ImVec2(cursorPos.x + maxWidth, cursorPos.y + maxHeight),
			true
		);

		ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + maxWidth - 10);
		ImGui::TextUnformatted(text);
		ImGui::PopTextWrapPos();

		ImGui::PopClipRect();

		ImGui::SetCursorPos(ImVec2(cursorLocalPos.x, cursorLocalPos.y + maxHeight));
	}
}