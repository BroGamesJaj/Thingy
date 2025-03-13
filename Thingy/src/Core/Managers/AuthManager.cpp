#include "tpch.h"

#include "AuthManager.h"

namespace Thingy {
	AuthManager::AuthManager(NetworkManager& networkManager, MessageManager& messageManager) : m_NetworkManager(networkManager), m_MessageManager(messageManager) {
		m_MessageManager.Subscribe("expiredToken", "authManager", [this](const MessageData data) {
			RefreshTokens();
			});

		m_MessageManager.Subscribe("logout", "authManager", [this](const MessageData data) {
			StoreToken("accessToken", "-");
			StoreToken("refreshToken", "-");
			m_MessageManager.Publish("loggedIn", false);

			});

		m_MessageManager.Subscribe("loggedIn", "authManager", [this](const MessageData data) {
			if (data.type() == typeid(bool)) {
				bool loggedIn = std::any_cast<bool>(data);
				if (loggedIn) {
					FetchUser();
				} else {
					user = User();
				}
			}
			});

		m_MessageManager.Subscribe("updateUser", "authManager", [this](const MessageData data) {
			FetchUser();
			});

		m_MessageManager.Subscribe("changeDescription", "authManager", [this](const MessageData data) {
			if (data.type() == typeid(std::string)) {
				std::string newDesc = std::any_cast<std::string>(data);
				json jsonData = { {"Description", newDesc} };
				std::string url = "http://localhost:3000/users/" + std::to_string(user.userID);
				std::string token = "";
				RetrieveToken("accessToken", token);
				m_NetworkManager.PatchRequestAuth(url, jsonData, token);
				FetchUser();
			}
			});
	}

	AuthManager::~AuthManager() {}

	void AuthManager::FetchUser() {
		user = User();
		std::string url = "http://localhost:3000/auth/profile";
		std::string accessToken;
		RetrieveToken("accessToken", accessToken);
		std::string response = m_NetworkManager.GetRequestAuth(url, accessToken);
		if (response.find("UserID") != std::string::npos) {
			json userJson = json::parse(response);
			from_json(userJson, user);
		}
		m_MessageManager.Publish("userChanged", "");
	}

	uint8_t AuthManager::StoreToken(const std::string& tokenName, const std::string& token) {
		wchar_t* wtoken = new wchar_t[4096];
		MultiByteToWideChar(CP_UTF8, 0, tokenName.c_str(), -1, wtoken, 4096);

		if (CredDeleteW(wtoken, CRED_TYPE_GENERIC, 0)) {
		} else {
			DWORD error = GetLastError();
			if (error != ERROR_NOT_FOUND) {
				T_ERROR("Failed to delete existing token for {0}.", tokenName);
				std::cerr << "Error code: " << error << "\n";
				delete[] wtoken;
				return 5;
			}
		}

		CREDENTIALW cred = { 0 };
		cred.Type = CRED_TYPE_GENERIC;
		cred.TargetName = wtoken;
		cred.CredentialBlobSize = token.size();
		cred.CredentialBlob = (LPBYTE)token.c_str();
		cred.Persist = CRED_PERSIST_LOCAL_MACHINE;

		if (CredWriteW(&cred, 0)) {
			delete[] wtoken;
			return 0;
		} else {
			DWORD error = GetLastError();
			delete[] wtoken;
			T_ERROR("Failed to store token {0}.", tokenName);
			switch (error) {
			case ERROR_ACCESS_DENIED:
				T_ERROR("Error: Access is denied.");
				return 1;
			case ERROR_INVALID_PARAMETER:
				T_ERROR("Error: Invalid parameter.");
				return 2;
			case ERROR_ALREADY_EXISTS:
				T_ERROR("Error: The credential already exists.");
				return 3;
			default:
				T_ERROR("Error: Unknown error.");
				return 4;
			}
		}
		
	}

	uint8_t AuthManager::RetrieveToken(const std::string& tokenName, std::string& token) {
		wchar_t* wtoken = new wchar_t[4096];
		MultiByteToWideChar(CP_UTF8, 0, tokenName.c_str(), -1, wtoken, 4096);

		PCREDENTIALW pcred;
		if (CredReadW(wtoken, CRED_TYPE_GENERIC, 0, &pcred)) {
			token = (char*)pcred->CredentialBlob;
			CredFree(pcred);
			delete[] wtoken;
			return 0;
		} else {
			DWORD error = GetLastError();
			T_ERROR("Failed to retrieve token {0}.", tokenName);
			delete[] wtoken;

			switch (error) {
			case ERROR_NOT_FOUND:
				T_ERROR("Error: The credential was not found.");
				return 1;
			case ERROR_ACCESS_DENIED:
				T_ERROR("Error: Access is denied.");
				return 2;
			case ERROR_INVALID_PARAMETER:
				T_ERROR("Error: Invalid parameter.");
				return 3;
			default:
				T_ERROR("Error: Unknown error.");
				return 4;
			}
		}

	}



	void AuthManager::RefreshTokens() {
		std::string url = "http://localhost:3000/auth/refresh";
		std::string refreshToken;
		if(RetrieveToken("refreshToken", refreshToken) != 0){
			m_MessageManager.Publish("loggedIn", false);
			return;
		};
		std::string response = m_NetworkManager.GetRequestAuth(url, refreshToken);
		if (response == "Received 401 Unauthorized") {
			m_MessageManager.Publish("changeScene", std::string("LoginScene"));
			m_MessageManager.Publish("loggedIn", false);
			return;
		}
		json parsedJSON = json::parse(response);
		std::string accessToken = parsedJSON["accessToken"];
		StoreToken("accessToken", accessToken);
		std::string newRefreshToken = parsedJSON["refreshToken"];
		StoreToken("refreshToken", newRefreshToken);
		m_MessageManager.Publish("loggedIn", true);
	}
}