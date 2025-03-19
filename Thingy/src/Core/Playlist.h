#pragma once
#include "tpch.h"

#include "Core\Image.h"

#include <SDL3\SDL.h>
#include <nlohmann\json.hpp>

using json = nlohmann::json;

namespace Thingy {

	struct Playlist {
		int playlistID = -1;
		int ownerID = -1;
		std::string playlistName = "";
		std::vector<uint8_t> playlistCoverBuffer;
		std::string description = "";
		bool priv = false;
		std::vector<int> trackIDs;
	};

	inline void from_json(const json& j, Playlist& p) {
		if (j.at("PlaylistID").is_string()) {
			p.playlistID = std::stoi(j.at("PlaylistID").get<std::string>());
		} else {
			j.at("PlaylistID").get_to(p.playlistID);
		}
		if (j.contains("OwnerID")) {
			j.at("OwnerID").get_to(p.ownerID);
		}
		if (j.contains("PlaylistName")) {
			j.at("PlaylistName").get_to(p.playlistName);
		}

		if (j.contains("PlaylistCover")) {
			if (!j.at("PlaylistCover").is_null()) {
				auto coverData = j["PlaylistCover"]["data"].get<std::vector<uint8_t>>();
				p.playlistCoverBuffer = coverData;
			}
		}

		if (j.contains("Description")) {
			if (!j.at("Description").is_null()) {
				j.at("Description").get_to(p.description);
			}
		}

		if (j.contains("Private")) {
			if (j.at("Private").is_number()) {
				p.priv = j.at("Private").get<int>() == 0 ? false : true;
			} else {
				j.at("Private").get_to(p.priv);
			}
		}
		
		if (j.contains("Tracks")) {
			for (size_t i = 0; i < j["Tracks"].size(); i++) {
				p.trackIDs.push_back(j["Tracks"][i]["SongID"]);
			}
		}
		
	}
}