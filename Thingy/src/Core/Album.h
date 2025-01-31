#pragma once
#include "tpch.h"
#include "Track.h"
#include <nlohmann\json.hpp>

using json = nlohmann::json;

namespace Thingy {
	struct Album {
		int id = 0;
		std::string name = "";
		std::string releaseDate = "";
		int artistID = 0;
		std::string artistName = "";
		std::string imageURL = "";
		std::vector<Track> tracks;

		inline std::string toString() const {
			std::ostringstream oss;
			oss << "Album Details:\n"
				<< "ID: " << id << "\n"
				<< "Name: " << name << "\n"
				<< "Release Date: " << releaseDate << "\n"
				<< "Artist ID: " << artistID << "\n"
				<< "Artist Name: " << artistName << "\n"
				<< "Image URL: " << imageURL << "\n"
				<< "Tracks: " << tracks.size() << " tracks";
			return oss.str();
		}
	};

	inline void to_json(json& j, const Album& a) {
		j = json{
			{"id", a.id},
			{"name", a.name},
			{"release_date", a.releaseDate},
			{"artist_id", a.artistID},
			{"artist_name", a.artistName},
			{"image_url", a.imageURL},
			{"tracks", a.tracks}
		};
	}

	inline void from_json(const json& j, Album& a) {
		if (j.at("id").is_string()) {
			a.id = std::stoi(j.at("id").get<std::string>());
		}
		else {
			j.at("id").get_to(a.id);
		}

		if (j.contains("artist_id")) {
			if (j.at("artist_id").is_string()) {
				a.artistID = std::stoi(j.at("artist_id").get<std::string>());
			}
			else {
				j.at("artist_id").get_to(a.artistID);
			}
		} else {
			a.artistID = -1;
		}
		

		j.at("name").get_to(a.name);
		j.at("releasedate").get_to(a.releaseDate);
		if (j.contains("artist_name")) {
			j.at("artist_name").get_to(a.artistName);
		} else {
			a.artistName = "";
		}
		j.at("image").get_to(a.imageURL);
	}
}