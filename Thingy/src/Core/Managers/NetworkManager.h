#pragma once
#include "tpch.h"
#include <curl\curl.h>
#include <SDL3\SDL.h>
#include "Core\Track.h"
#include "Core\Album.h"
#include "Core\Artist.h"
#include <nlohmann\json.hpp>

using json = nlohmann::json;

namespace Thingy {
	struct MemoryChunk {
		std::vector<unsigned char> data;

		static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
			size_t totalSize = size * nmemb;
			auto* mem = static_cast<MemoryChunk*>(userp);
			mem->data.insert(mem->data.end(), (unsigned char*)contents, (unsigned char*)contents + totalSize);
			return totalSize;
		}
	};

	class NetworkManager {
	public:
		NetworkManager();
		~NetworkManager();

		NetworkManager(const NetworkManager&) = delete;
		void operator=(const NetworkManager&) = delete;

		void CleanUp();

		static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
			size_t totalSize = size * nmemb;
			std::vector<uint8_t>* buffer = static_cast<std::vector<uint8_t>*>(userp);
			buffer->insert(buffer->end(), static_cast<uint8_t*>(contents), static_cast<uint8_t*>(contents) + totalSize);
			return totalSize;
		}

		static size_t curl_callback(void* ptr, size_t size, size_t nmemb, std::string* data) {
			data->append((char*)ptr, size * nmemb);
			return size * nmemb;
		}


		bool DownloadAudio(std::string& url, std::vector<uint8_t>& buffer);
		void DownloadImage(std::string& url, std::vector<unsigned char>& imageData);

		void CleanupGet(CURL* curl, curl_slist* headers);

		std::string GetRequest(std::string& url);

		std::vector<Track> GetTrack(std::string url);
		std::vector<Album> GetAlbum(std::string url);
		std::vector<Artist> GetArtist(std::string url);



	private:
	};
}