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
		ImGui::TextUnformatted(reinterpret_cast<const char*>(u8"%s",text));
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

	bool OpenFileExplorer(std::string& filePath) {
		bool success = true;
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr)) {

			IFileOpenDialog* pFileOpen;
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
			if (SUCCEEDED(hr)) {

				COMDLG_FILTERSPEC fileTypes[] = {
				{L"Image Files", L"*.jpg;*.jpeg;*.png;*.bmp"},
				{L"JPEG Files", L"*.jpg;*.jpeg"},
				{L"PNG Files", L"*.png"},
				{L"BMP Files", L"*.bmp"},
				};
				pFileOpen->SetFileTypes(ARRAYSIZE(fileTypes), fileTypes);

				hr = pFileOpen->Show(NULL);
				if (SUCCEEDED(hr)) {

					IShellItem* pItem;
					hr = pFileOpen->GetResult(&pItem);
					if (SUCCEEDED(hr)) {

						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
						if (SUCCEEDED(hr)) {

							char path[MAX_PATH];
							wcstombs(path, pszFilePath, MAX_PATH);
							filePath = path;
							CoTaskMemFree(pszFilePath);
						} else {
							success = false;
						}
						pItem->Release();
					} else {
						success = false;
					}
				} else {
					success = false;
				}
				pFileOpen->Release();
			} else {
				success = false;
			}
			CoUninitialize();
		}
		return success;
	}

	std::string GetFileExtension(const std::string& filePath) {
		size_t dotPos = filePath.find_last_of(".");
		if (dotPos != std::string::npos) {
			std::string ext = filePath.substr(dotPos + 1);
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
			return ext;
		}
		return "";
	}

	bool IsImageFile(const std::string& filePath) {
		std::string ext = GetFileExtension(filePath);
		return (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "bmp");
	}

	void CircleImage(ImTextureID user_texture_id, float diameter, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col) {
		ImVec2 p_min = ImGui::GetCursorScreenPos();
		ImVec2 p_max = ImVec2(p_min.x + diameter, p_min.y + diameter);
		std::string name = std::to_string(p_min.x) + std::to_string(diameter);
		ImGui::InvisibleButton(name.data(), ImVec2(diameter,diameter));
		ImGui::GetWindowDrawList()->AddImageRounded(user_texture_id, p_min, p_max, uv0, uv1, ImGui::GetColorU32(tint_col), diameter * 0.5f);
	}
}