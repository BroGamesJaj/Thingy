#include "tpch.h"

#include "LoginModule.h"

namespace Thingy{
	void LoginModule::SetupSubscriptions() {}

	void LoginModule::OnLoad(const std::variant<int, std::string> moduleState) {
		reg = false;
		email = "";
		password = "";
		username = "";
		error = "";
	}

	void LoginModule::OnUpdate() {}

	void LoginModule::Window() {
		if (reg){
			ImGui::Text("Sign-up");
			ImGui::Text("Email:");
			ImGui::SameLine();
			ImGui::InputText("##email", &email, 0, ResizeCallback, (void*)&email);
			ImGui::Text("Username:");
			ImGui::SameLine();
			ImGui::InputText("##username", &username, 0, ResizeCallback, (void*)&username);
			ImGui::Text("Password:");
			ImGui::SameLine();
			ImGui::InputText("##password", &password, ImGuiInputTextFlags_Password, ResizeCallback, (void*)&password);

			if (ImGui::Button("Sign-up")) {
				std::regex pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
				if (email == "") {
					error = "Email cannot be empty!";
				} else if (username == "") {
					error = "Username cannot be empty!";
				} else if (password == "") {
					error = "Password cannot be empty!";
				} else if (!std::regex_match(email, pattern)) {
					error = "Email must be an email address!";
				} else {
					std::string url = "http://localhost:3000/users/register";
					json payload = { { "Email", email }, { "Username", username }, { "Password", password }};
					std::string response = m_NetworkManager.PostRequest(url, payload);
					T_TRACE("response: {0}", response);
					if (response == "Email already in use") {
						error = "Email already in use!";
					} else if (response == "Username already in use") {
						error = "Username is already in use!";
					} else {
						error = "";
						email = "";
						username = "";
						password = "";
						reg = false;
					}
				}
			}
			ImGui::Text("You already have an account?");
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.231f, 0.51f, 0.965f, 1.0f));

			ImGui::Text("Login");
			if (ImGui::IsItemHovered()) {
				upProps |= BIT(3);
			}
			if (ImGui::IsItemClicked()) {
				T_TRACE("clicked login");
				reg = false;
			}
			ImGui::PopStyleColor();
		} else {

			ImGui::Text("Login");
			ImGui::Text("Email:");
			ImGui::SameLine();
			ImGui::InputText("##email", &email, 0, ResizeCallback, (void*)&email);
			ImGui::Text("Password:");
			ImGui::SameLine();
			ImGui::InputText("##password", &password, ImGuiInputTextFlags_Password, ResizeCallback, (void*)&password);

			if (ImGui::Button("Login")) {
				std::string url = "http://localhost:3000/auth/login";
				json payload = {{ "Email", email}, { "Password", password}};
				std::string response = m_NetworkManager.PostRequest(url, payload);
				if (response == "Received 401 Unauthorized") {
					error = "Incorrect email or password!";
				} else {
					error = "";
					json parsed = json::parse(response);
					std::string accessToken = parsed["accessToken"];
					std::string refreshToken = parsed["refreshToken"];
					T_INFO("accessToken: {0}", accessToken);
					
					T_INFO("refreshToken: {0}", refreshToken);
					m_AuthManager.StoreToken("accessToken", accessToken);
					m_AuthManager.StoreToken("refreshToken", refreshToken);
					std::string tk1, tk2;
					m_AuthManager.RetrieveToken("accessToken", tk1);
					m_AuthManager.RetrieveToken("refreshToken", tk2);
					T_INFO("accessToken: {0}", tk1);
					T_INFO("refreshToken: {0}", tk2);
					email = "";
					password = "";
					username = "";
					m_MessageManager.Publish("loggedIn", true);
					m_MessageManager.Publish("changeScene", std::string("FrontPage"));
				}
			}
		

			ImGui::Text("Don't have an account?");
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.231f, 0.51f, 0.965f, 1.0f));
			
			ImGui::Text("Sign-up");
			if (ImGui::IsItemHovered()) {
				upProps |= BIT(3);
			}
			if (ImGui::IsItemClicked()) {
				T_TRACE("clicked sign-up");
				reg = true;
			}
			ImGui::PopStyleColor();
		}

		if (error != "") {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			ImGui::Text(error.c_str());
			ImGui::PopStyleColor();
		}
	}

	uint16_t LoginModule::OnRender() {
		upProps &= BIT(0);
		ImGui::Begin(GetModuleName().c_str(), 0, defaultWindowFlags);
		Window();
		ImGui::End();
		return upProps;
	}
}