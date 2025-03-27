#pragma once
#include "tpch.h"
#include "Playlist.h"

namespace Thingy {

	enum FollowedType {
		PLAYLIST,
		ARTIST,
		ALBUM

	};

	struct User {
		int userID = -1;
		std::string username = "";
		std::string email = "";
		std::string description = "";

		std::vector<uint8_t> pfpBuffer;
		std::vector<Playlist> playlists;
		std::vector<std::tuple<int, FollowedType, int>> followed;

		inline std::string toString() const {
			std::ostringstream oss;
			oss << "User Details:\n"
				<< "ID: " << userID << "\n"
				<< "Username: " << username << "\n"
				<< "Email: " << email << "\n"
				<< "Description: " << description << "\n"
				<< "Playlists count: " << playlists.size() << "\n";
			return oss.str();
		}

	};

	inline void from_json(const json& j, User& u) {
		if (j.at("UserID").is_string()) {
			u.userID = std::stoi(j.at("UserID").get<std::string>());
		} else {
			j.at("UserID").get_to(u.userID);
		}

		if (j.contains("Username")) {
			j.at("Username").get_to(u.username);
		}

		if (j.contains("Email")) {
			j.at("Email").get_to(u.email);
		}

		if (j.contains("Pfp")) {
			if (!j.at("Pfp").is_null()) {
				auto pfpData = j["Pfp"]["data"].get<std::vector<uint8_t>>();
				u.pfpBuffer = pfpData;
			}
		}

		if (j.contains("Description")) {
			if (!j.at("Description").is_null()) {
				j.at("Description").get_to(u.description);
			}
		}

		if (j.contains("Playlists")) {
			for (size_t i = 0; i < j.at("Playlists").size(); i++) {
				Playlist p;
				from_json(j["Playlists"][i], p);
				u.playlists.push_back(p);
			}
		}
	}
}