#pragma once
#include <curl\curl.h>
#include <SDL3\SDL.h>
namespace Thingy {
	class NetworkManager {
	public:
		NetworkManager();
		~NetworkManager();

		NetworkManager(const NetworkManager&) = delete;
		void operator=(const NetworkManager&) = delete;

		static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
			size_t totalSize = size * nmemb;
			std::vector<uint8_t>* buffer = static_cast<std::vector<uint8_t>*>(userp);
			buffer->insert(buffer->end(), static_cast<uint8_t*>(contents), static_cast<uint8_t*>(contents) + totalSize);
			return totalSize;
		}

		bool DownloadFile(const std::string& url, std::vector<uint8_t>& buffer);

	private:
	};
}