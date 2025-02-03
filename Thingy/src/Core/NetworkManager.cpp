#include "tpch.h"
#include "NetworkManager.h"
#include "Core\Log.h"

namespace Thingy {
	static void URLSanitizer(std::string& url) {
		url.erase(std::remove(url.begin(), url.end(), '\\'), url.end());
		url.erase(std::remove_if(url.begin(), url.end(), ::isspace), url.end());
	}

	NetworkManager::NetworkManager() {
		if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
			fprintf(stderr, "curl_global_init() failed!\n");
		}
	}

	NetworkManager::~NetworkManager() {
	}

	void NetworkManager::CleanUp() {
		curl_global_cleanup();
	}

	void NetworkManager::CleanupGet(CURL* curl, curl_slist* headers) {
		if (curl)
			curl_easy_cleanup(curl);
		if (headers)
			curl_slist_free_all(headers);
	}

	

	bool NetworkManager::DownloadAudio(std::string& url, std::vector<uint8_t>& buffer) {
		buffer.clear();
		CURL* curl = curl_easy_init();
		if (!curl) {
			SDL_Log("Failed to initialize CURL.");
			return false;
		}
		URLSanitizer(url);
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

	void NetworkManager::DownloadImage(std::string& url, std::vector<unsigned char>& imageData) {
		CURL* curl = curl_easy_init();
		if (!curl) throw std::runtime_error("Failed to initialize cURL");

		URLSanitizer(url);
		MemoryChunk chunk;
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, MemoryChunk::WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			curl_easy_cleanup(curl);
			throw std::runtime_error("cURL request failed: " + std::string(curl_easy_strerror(res)));
		}
		
		curl_easy_cleanup(curl);
		imageData = std::move(chunk.data);
	}


	std::string NetworkManager::GetRequest(std::string& url) {
		URLSanitizer(url);
		CURL* curl = curl_easy_init();
		std::string response_string;
		struct curl_slist* headers = NULL;
		if (!curl)
		{
			std::cout << "ERROR : Curl initialization\n" << std::endl;
			CleanupGet(curl, headers);
			return NULL;
		}

		headers = curl_slist_append(headers, "User-Agent: libcurl-agent/1.0");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "Cache-Control: no-cache");

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);


		CURLcode status = curl_easy_perform(curl);
		if (status != 0)
		{
			std::cout << "Error: Request failed on URL : " << url << std::endl;
			std::cout << "Error Code: " << status << " Error Detail : " << curl_easy_strerror(status) << std::endl;
			CleanupGet(curl, headers);
			return "curl error";
		}

		curl_easy_cleanup(curl);
		curl_slist_free_all(headers);

		return response_string;
	}

	std::vector<Track> NetworkManager::GetTrack(std::string& url) {
		std::string jsonData = GetRequest(url);
		if (jsonData == "curl error" || jsonData.find("error code:") != std::string::npos) {
			T_ERROR("{0}", jsonData);
			return std::vector<Track>();
		}
		json parsedJsonData = json::parse(jsonData);
		std::vector<Track> tracks;
		for (size_t i = 0; i < parsedJsonData["headers"]["results_count"]; i++) {
			Track track = parsedJsonData["results"][i];
			tracks.push_back(track);
		}
		return tracks;
	}

	std::vector<Album> NetworkManager::GetAlbum(std::string& url) {
		std::string jsonData = GetRequest(url);
		if (jsonData == "curl error" || jsonData.find("error code:") != std::string::npos) {
			T_ERROR("{0}", jsonData);
			return std::vector<Album>();
		}
		json parsedJsonData = json::parse(jsonData);
		std::vector<Album> albums;
		for (size_t i = 0; i < parsedJsonData["headers"]["results_count"]; i++) {
			Album album = parsedJsonData["results"][i];
			
			for (size_t j = 0; j < parsedJsonData["results"][0]["tracks"].size(); j++)	{
				Track track = parsedJsonData["results"][i]["tracks"][j];
				track.albumName = album.name;
				track.albumID = album.id;
				track.albumImageURL = album.imageURL;
				track.imageURL = album.imageURL;
				track.artistID = album.artistID;
				track.artistName = album.artistName;
				track.releaseDate = album.releaseDate;
				album.tracks.push_back(track);
			}
			albums.push_back(album);
		}
		return albums;
	}

	std::vector<Artist> NetworkManager::GetArtist(std::string& url) {
		std::string jsonData = GetRequest(url);
		if(jsonData == "curl error" || jsonData.find("error code:") != std::string::npos) {
			T_ERROR("{0}", jsonData);
			return std::vector<Artist>();
		}
		json parsedJsonData = json::parse(jsonData);
		std::vector<Artist> artists;
		for (size_t i = 0; i < parsedJsonData["headers"]["results_count"]; i++) {
			Artist artist = parsedJsonData["results"][i];

			for (size_t j = 0; j < parsedJsonData["results"][i]["albums"].size(); j++) {
				Album album = parsedJsonData["results"][i]["albums"][j];
				album.artistID = artist.id;
				album.artistName = artist.artistName;
				artist.albums.push_back(album);
			}
			artists.push_back(artist);
		}
		return artists;
	}
}