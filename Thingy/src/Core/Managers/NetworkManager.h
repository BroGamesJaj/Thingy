#pragma once
#include "tpch.h"
#include <curl\curl.h>
#include <SDL3\SDL.h>
#include "Core\Track.h"
#include "Core\Album.h"
#include "Core\Artist.h"
#include <nlohmann\json.hpp>

#include "MessageManager.h"

using json = nlohmann::json;

namespace Thingy {
	struct MemoryChunk {
		std::vector<unsigned char> data;

		static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
			size_t totalSize = size * nmemb;
			auto* mem = static_cast<MemoryChunk*>(userp);
			mem->data.insert(mem->data.end(), static_cast<unsigned char*>(contents), static_cast<unsigned char*>(contents) + totalSize);
			return totalSize;
		}
	};

	class NetworkManager {
	public:
		NetworkManager(MessageManager& messageManager);
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

		static size_t WriteCallbackPost(void* contents, size_t size, size_t nmemb, void* userp) {
			if (!contents || !userp) return 0;

			const size_t totalSize = size * nmemb;
			auto* buffer = static_cast<std::string*>(userp);
			buffer->append(static_cast<char*>(contents), totalSize);
			return totalSize;
		}

		static size_t curl_callback(void* ptr, size_t size, size_t nmemb, std::string* data) {
			data->append(static_cast<char*>(ptr), size * nmemb);
			return size * nmemb;
		}


		bool DownloadAudio(std::string& url, std::vector<uint8_t>& buffer);
		void DownloadImage(std::string& url, std::vector<unsigned char>& imageData);

		void CleanupGet(CURL* curl, curl_slist* headers);

		std::string GetRequest(std::string& url);
		std::string GetRequestAuth(std::string& url, const std::string& token);

		std::string PostRequest(std::string& url, json& data);

		std::string PatchRequestAuth(std::string& url, const json& data, const std::string& token);

		std::string UploadImage(std::string& url, const std::string& filePath, const std::string& token);

		std::string AddPlaylist(std::string& url, const std::string& playlistName, const std::string& desc, const bool& isPrivate, const std::string& filePath, const std::string& token);

		std::vector<Track> GetTrack(std::string url);
		std::vector<Album> GetAlbum(std::string url);
		std::vector<Artist> GetArtist(std::string url);
		std::vector<Artist> GetArtistsWithTracks(std::string url);

		std::unordered_map<std::string, std::vector<std::pair<std::string, int>>>  GetAutoComplete(std::string input);



	private:
		MessageManager& m_MessageManager;

	};
}