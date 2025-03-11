#pragma once
#include "tpch.h"

#include "NetworkManager.h"
#include "MessageManager.h"

#include "Core\User.h"

namespace Thingy {
	class AuthManager {
	public:
		AuthManager(std::unique_ptr<NetworkManager>& networkManager, std::unique_ptr<MessageManager>& messageManager);
		~AuthManager();

		const User& GetUser() { return user; };

		uint8_t StoreToken(const std::string& tokenName, const std::string& token);
		uint8_t RetrieveToken(const std::string& tokenName, std::string& token);

		void RefreshTokens();
	private:
		void FetchUser();
		std::unique_ptr<NetworkManager>& m_NetworkManager;
		std::unique_ptr<MessageManager>& m_MessageManager;

		User user;

	};
}