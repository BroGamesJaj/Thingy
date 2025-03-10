#pragma once
#include "tpch.h"

#include "NetworkManager.h"
#include "MessageManager.h"

namespace Thingy {
	class AuthManager {
	public:
		AuthManager(std::unique_ptr<NetworkManager>& networkManager, std::unique_ptr<MessageManager>& messageManager);
		~AuthManager();

		uint8_t StoreToken(const std::string& token, const std::string& tokenName);
		uint8_t RetrieveToken(const std::string& tokenName, std::string& token);

		void RefreshTokens();
	private:
		std::unique_ptr<NetworkManager>& m_NetworkManager;
		std::unique_ptr<MessageManager>& m_MessageManager;
	};
}