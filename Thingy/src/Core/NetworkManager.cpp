#include "tpch.h"
#include "NetworkManager.h"
#include "Core\Log.h"

namespace Thingy {
	NetworkManager::NetworkManager() {
	}

	NetworkManager::~NetworkManager() {
	}
	bool NetworkManager::DownloadFile(const std::string& url, std::vector<uint8_t>& buffer) {
		buffer.clear();
		CURL* curl = curl_easy_init();
		if (!curl) {
			SDL_Log("Failed to initialize CURL.");
			return false;
		}
		T_INFO("{0}", url.c_str());
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			SDL_Log("CURL failed: %s\n", curl_easy_strerror(res));
			buffer.clear();
		}
		curl_easy_cleanup(curl);

		return (res == CURLE_OK);
	}
}