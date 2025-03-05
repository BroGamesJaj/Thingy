#include "tpch.h"
#include "HelperFunctions.h"

namespace Thingy {
	void LimitedTextWrap(const char* text, float maxWidth, int maxLines) {

		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImVec2 cursorLocalPos = ImGui::GetCursorPos();

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

	std::string SecondsToTimeString(int seconds) {
		if (seconds <= 0) return "0s";

		int hours = seconds / 3600;
		seconds %= 3600;
		int minutes = seconds / 60;
		seconds %= 60;

		std::string result;

		if (hours > 0) {
			result += std::to_string(hours) + "hr ";
		}
		if (minutes > 0 || hours > 0) {
			result += std::to_string(minutes) + "min ";
		}
		result += std::to_string(seconds) + "sec";

		return result;
	}
}