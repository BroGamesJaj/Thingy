#pragma once
#include "tpch.h"

#include "NetworkManager.h"
#include "MessageManager.h"

#include "Core\User.h"

namespace Thingy {
	class AuthManager {
	public:
		AuthManager(NetworkManager& networkManager, MessageManager& messageManager);
		~AuthManager();

		const User& GetUser() noexcept {
			return user; 
		};

		uint8_t StoreToken(const std::string& tokenName, const std::string& token);
		uint8_t RetrieveToken(const std::string& tokenName, std::string& token);

		void RefreshTokens();
	private:
		void FetchUser();
		void UpdateFollowed();

		NetworkManager& m_NetworkManager;
		MessageManager& m_MessageManager;

		User user;

	};
}