#include "tpch.h"
#include "NetworkManager.h"
#include "Core\Log.h"

namespace Thingy {
	static void URLSanitizer(std::string& url) {
		url.erase(std::remove(url.begin(), url.end(), '\\'), url.end());
		url.erase(std::remove_if(url.begin(), url.end(), [](unsigned char c) { return std::isspace(c); }), url.end());
	}

	NetworkManager::NetworkManager(MessageManager& messageManager) : m_MessageManager(messageManager) {
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
		std::string responseString;
		long responseCode;

		struct curl_slist* headers = NULL;
		if (!curl)
		{
			std::cout << "ERROR : Curl initialization\n" << std::endl;
			CleanupGet(curl, headers);
			return "";
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
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);


		CURLcode status = curl_easy_perform(curl);
		if (status != 0) {
			std::cout << "Error: Request failed on URL : " << url << std::endl;
			std::cout << "Error Code: " << status << " Error Detail : " << curl_easy_strerror(status) << std::endl;
			CleanupGet(curl, headers);
			return "curl error";
		} else {
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
			if (responseCode == 403) {
				std::cout << "Received 403 Forbidden" << std::endl;
				return "Received 403 Forbidden";
			}
		}

		curl_easy_cleanup(curl);
		curl_slist_free_all(headers);

		return responseString;
	}



	std::string NetworkManager::GetRequestAuth(std::string& url, const std::string& token) {
		URLSanitizer(url);
		CURL* curl = curl_easy_init();
		std::string responseString;
		long responseCode;
		
		struct curl_slist* headers = NULL;
		if (!curl) {
			std::cout << "ERROR : Curl initialization\n" << std::endl;
			CleanupGet(curl, headers);
			return "";
		}

		headers = curl_slist_append(headers, "User-Agent: libcurl-agent/1.0");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "Cache-Control: no-cache");
		headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);


		CURLcode status = curl_easy_perform(curl);
		if (status != 0) {
			std::cout << "Error: Request failed on URL : " << url << std::endl;
			std::cout << "Error Code: " << status << " Error Detail : " << curl_easy_strerror(status) << std::endl;
			CleanupGet(curl, headers);
			return "curl error";
		} else {
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

			if (responseCode == 403) {
				T_ERROR("Received 403 Forbidden.");
				return "Received 403 Forbidden";
			}
			if (responseCode == 401) {
				T_ERROR("Received 401 Unauthorized.");
				if (url.find("auth/refresh") != std::string::npos){
					m_MessageManager.Publish("changeScene", std::string("LoginScene"));
				} else {
					T_INFO("Token was expired.");
					m_MessageManager.Publish("expiredToken", "");
				};
				return "Received 401 Unauthorized";
			}


		}

		curl_easy_cleanup(curl);
		curl_slist_free_all(headers);

		return responseString;
	}

	std::string NetworkManager::PostRequest(std::string& url, json& data) {
		URLSanitizer(url);
		CURL* curl = curl_easy_init();
		std::string response;
		struct curl_slist* headers = nullptr;
		long responseCode;

		if (!curl) {
			std::cout << "ERROR : Curl initialization\n" << std::endl;
			CleanupGet(curl, headers);
			return "";
		}

		headers = curl_slist_append(headers, "Content-Type: application/json");

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		std::string json_str = data.dump();
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackPost);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			response = "Curl error: " + std::string(curl_easy_strerror(res));
		} else {
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
			if (responseCode == 401) {
				return "Received 401 Unauthorized";
			}
			if (responseCode == 409) {
				return json::parse(response)["message"];
			}
		};
		curl_easy_cleanup(curl);
		curl_slist_free_all(headers);

		return response;
	}

	std::string NetworkManager::PatchRequestAuth(std::string& url, const json& data, const std::string& token) {
		URLSanitizer(url);
		CURL* curl = curl_easy_init();
		std::string response;
		struct curl_slist* headers = nullptr;
		long responseCode;

		if (!curl) {
			std::cout << "ERROR : Curl initialization\n" << std::endl;
			CleanupGet(curl, headers);
			return "";
		}

		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		std::string json_str = data.dump();
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackPost);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			response = "Curl error: " + std::string(curl_easy_strerror(res));
		} else {
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
			if (responseCode == 401) {
				return "Received 401 Unauthorized";
			}
			if (responseCode == 409) {
				return json::parse(response)["message"];
			}
		};
		curl_easy_cleanup(curl);
		curl_slist_free_all(headers);

		return response;
	}

	std::string NetworkManager::UploadImage(std::string& url, const std::string& filePath, const std::string& token){
		URLSanitizer(url);
		CURL* curl = curl_easy_init();
		std::string response;
		struct curl_slist* headers = nullptr;
		long responseCode;

		if (!curl) {
			std::cout << "ERROR : Curl initialization\n" << std::endl;
			CleanupGet(curl, headers);
			return "";
		}
		struct curl_httppost* form = NULL;
		struct curl_httppost* last = NULL;

		curl_formadd(&form, &last,
			CURLFORM_COPYNAME, "file",
			CURLFORM_FILE, filePath.c_str(),
			CURLFORM_END);
		
		headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());	

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, form);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			std::cout << "Curl error: " << curl_easy_strerror(res) << std::endl;
		} else {
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
			if (responseCode == 401) {
				return "Received 401 Unauthorized";
			}
		};
		curl_easy_cleanup(curl);
		curl_slist_free_all(headers);

		return response;
	}

	std::string NetworkManager::AddPlaylist(std::string& url, const std::string& playlistName, const std::string& desc, const bool& isPrivate, const std::string& filePath, const std::string& token) {
		URLSanitizer(url);
		CURL* curl = curl_easy_init();
		std::string response;
		struct curl_slist* headers = nullptr;
		long responseCode;
		T_TRACE("hello");
		if (!curl) {
			std::cout << "ERROR : Curl initialization\n" << std::endl;
			CleanupGet(curl, headers);
			return "";
		}

		curl_mime* mime = curl_mime_init(curl);

		curl_mimepart* part = curl_mime_addpart(mime);
		curl_mime_name(part, "PlaylistName");
		curl_mime_data(part, playlistName.c_str(), CURL_ZERO_TERMINATED);

		if (!desc.empty()) {
			part = curl_mime_addpart(mime);
			curl_mime_name(part, "Description");
			curl_mime_data(part, desc.c_str(), CURL_ZERO_TERMINATED);
		}

		part = curl_mime_addpart(mime);
		curl_mime_name(part, "Private");
		curl_mime_data(part, isPrivate ? "true" : "false", CURL_ZERO_TERMINATED);

		if (!filePath.empty()) {
			part = curl_mime_addpart(mime);
			curl_mime_name(part, "PlaylistCover");
			curl_mime_filedata(part, filePath.c_str());
		}
		
		headers = curl_slist_append(headers, "Content-Type: multipart/form-data");
		headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			std::cout << "Curl error: " << curl_easy_strerror(res) << std::endl;
		} else {
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
			if (responseCode == 401) {
				return "Received 401 Unauthorized";
			}
		};
		curl_mime_free(mime);
		curl_easy_cleanup(curl);
		curl_slist_free_all(headers);

		return response;
	}

	std::vector<Track> NetworkManager::GetTrack(std::string url) {
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

	std::vector<Album> NetworkManager::GetAlbum(std::string url) {
		std::string jsonData = GetRequest(url);
		if (jsonData == "curl error" || jsonData.find("error code:") != std::string::npos) {
			T_ERROR("{0}", jsonData);
			return std::vector<Album>();
		}
		json parsedJsonData = json::parse(jsonData);
		std::vector<Album> albums;
		for (size_t i = 0; i < parsedJsonData["headers"]["results_count"]; i++) {
			Album album = parsedJsonData["results"][i];
			for (size_t j = 0; j < parsedJsonData["results"][i]["tracks"].size(); j++)	{
				Track track = parsedJsonData["results"][i]["tracks"][j];
				track.albumName = album.name;
				track.albumID = album.id;
				track.albumImageURL = album.imageURL;
				track.imageURL = album.imageURL;
				track.artistID = album.artistID;
				track.artistName = album.artistName;
				track.releaseDate = album.releaseDate;
				if (parsedJsonData["results"][i]["tracks"].size() == 1) {
					album.imageURL = track.imageURL;
				}
				album.tracks.push_back(track);
			}
			album.trackCount = parsedJsonData["results"][i].size();
			albums.push_back(album);
		}
		return albums;
	}

	std::vector<Artist> NetworkManager::GetArtist(std::string url) {
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

	//https://api.jamendo.com/v3.0/artists/tracks/
	std::vector<Artist> NetworkManager::GetArtistsWithTracks(std::string url) {
		std::string jsonData = GetRequest(url);
		if (jsonData == "curl error" || jsonData.find("error code:") != std::string::npos) {
			T_ERROR("{0}", jsonData);
			return std::vector<Artist>();
		}
		json parsedJsonData = json::parse(jsonData);
		json& headers = parsedJsonData["headers"];
		if (headers["status"] != "success") {
			T_ERROR("{0}", jsonData);
			T_ERROR("code: {0}", headers["code"].get<int>());
			return std::vector<Artist>();
		}
		json& results = parsedJsonData["results"];
		std::vector<Artist> artists;
		for (size_t i = 0; i < headers["results_count"]; i++) {
			std::cout << "hello" << std::endl;
			json& currArtist = results[i];
			Artist artist = currArtist;
			std::unordered_map<uint32_t, Album> albums;
			for (size_t j = 0; j < currArtist["tracks"].size(); j++) {
				json& currTrack = currArtist["tracks"][j];
				Track track = currTrack;
				track.artistID = artist.id;
				track.artistName = artist.artistName;
				albums[track.albumID].tracks.push_back(track);
			}
			for (auto& albumPair : albums) {
				Album& album = albumPair.second;
				album.artistID = artist.id;
				album.artistName = artist.artistName;
				album.id = albumPair.first;
				album.imageURL = album.tracks[0].imageURL;
				album.name = album.tracks[0].albumName;
				album.releaseDate = album.tracks[0].releaseDate;
				album.trackCount = album.tracks.size();
				artist.albums.push_back(album);
			}
			artists.push_back(artist);
		}
		return artists;
	}

	std::unordered_map<std::string, std::vector<std::pair<std::string, int>>>  NetworkManager::GetAutoComplete(std::string input) {
		std::string url = 
			"https://api.jamendo.com/v3.0/autocomplete/?client_id="
			+ std::string(CLIENTID)
			+ "&format=jsonpretty&limit=5&matchcount=1&prefix="
			+ input;
		std::string jsonData = GetRequest(url);
		if (jsonData == "curl error" || jsonData.find("error code:") != std::string::npos || jsonData == "Received 403 Forbidden") {
			T_ERROR("{0}", jsonData);
			return std::unordered_map<std::string, std::vector<std::pair<std::string, int>>>();
		}
		json parsedJsonData = json::parse(jsonData);
		json& headers = parsedJsonData["headers"];
		if (headers["status"] != "success") {
			T_ERROR("{0}", jsonData);
			T_ERROR("code: {0}", headers["code"].get<int>());
			return std::unordered_map<std::string, std::vector<std::pair<std::string, int>>>();
		}
		json& results = parsedJsonData["results"];
		std::vector<std::string> terms = { "tags", "tracks", "albums", "artists" };
		std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> termResults;
		for (auto& term : terms) {
			if (results.contains(term)) {
				for (size_t i = 0; i < results[term].size(); i++) {
					json& currTermResults = results[term][i];
					termResults[term].push_back(std::make_pair(currTermResults["match"].is_number() ? std::to_string(currTermResults["match"].get<int>()) : currTermResults["match"].get<std::string>(), currTermResults["count"]));
				}
			}
		}
		return termResults;

	}
}