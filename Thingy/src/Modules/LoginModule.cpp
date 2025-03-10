#include "tpch.h"

#include "LoginModule.h"

namespace Thingy{
	void LoginModule::SetupSubscriptions() {}

	void LoginModule::OnLoad(const std::variant<int, std::string> moduleState) {}

	void LoginModule::OnUpdate() {}

	void LoginModule::Window() {
		ImGui::Text("Login");
		ImGui::InputText("email", &email, 0, ResizeCallback, (void*)&email);
		ImGui::InputText("password", &password, ImGuiInputTextFlags_Password, ResizeCallback, (void*)&password);

		if (ImGui::Button("Login")) {
			std::string url = "http://localhost:3000/auth/login";
			json payload = {{ "Email", email}, { "Password", password}};
			std::string response = m_NetworkManager->PostRequest(url, payload);
			if (response == "Received 401 Unauthorized") {
				T_ERROR("Bad email or password");
			} else {
				json parsed = json::parse(response);
				std::string accessToken = parsed["accessToken"];
				std::string refreshToken = parsed["refreshToken"];
				m_AuthManager->StoreToken(accessToken, "accessToken");
				m_AuthManager->StoreToken(refreshToken, "refreshToken");
			}
		};
	}

	uint16_t LoginModule::OnRender() {
		ImGui::Begin(GetModuleName().data(), 0, defaultWindowFlags);
		Window();
		ImGui::End();
		return 0;
	}
}