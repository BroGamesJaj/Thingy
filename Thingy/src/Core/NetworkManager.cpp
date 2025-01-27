#include "tpch.h"
#include "NetworkManager.h"

namespace Thingy {
	NetworkManager::NetworkManager() {
	}

	NetworkManager::~NetworkManager() {
	}
	bool NetworkManager::DownloadFile(const std::string& url, std::vector<char>& buffer) {
		buffer.clear();
		CURL* curl = curl_easy_init();
		if (!curl) {
			SDL_Log("Failed to initialize CURL.");
			return false;
		}

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