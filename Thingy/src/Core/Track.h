#pragma once
#include "tpch.h"
#include <nlohmann\json.hpp>

using json = nlohmann::json;

namespace Thingy {

	struct Track {
		std::string title = "";
		int id = 0;
		int artistID = 0;
		int albumID = 0;
		int duration = 0;
		std::string imageURL = "";
		std::string artistName = "";
		std::string albumName = "";
		std::string albumImageURL = "";
		std::string releaseDate = "";
		std::string audioURL = "";

		inline std::string toString() const {
			std::ostringstream oss;
			oss << "Track Details:\n"
				<< "ID: " << id << "\n"
				<< "Title: " << title << "\n"
				<< "Duration: " << duration << " seconds\n"
				<< "Image URL: " << imageURL << "\n"
				<< "Artist ID: " << artistID << "\n"
				<< "Artist Name: " << artistName << "\n"
				<< "Album ID: " << albumID << "\n"
				<< "Album Name: " << albumName << "\n"
				<< "Album Image URL: " << albumImageURL << "\n"
				<< "Release Date: " << releaseDate << "\n"
				<< "Audio URL: " << audioURL;
			return oss.str();
		}
	};

	inline void to_json(json& j, const Track& t) {
		j = json{ {"id", t.id},
			{"name", t.title},
			{"duration", t.duration},
			{"image", t.imageURL}, 
			{"artist_id", t.artistID},
			{"artist_name", t.artistName},
			{"album_id", t.albumID},
			{"album_name", t.albumName}, 
			{"album_image", t.albumImageURL},
			{"releasedate", t.releaseDate},
			{"audio", t.audioURL}};
	}

	inline void from_json(const json& j, Track& t) {
		if(j.at("id").is_string()) {
				t.id = std::stoi(j.at("id").get<std::string>());
		} else {
		 j.at("id").get_to(t.id);
		}

		if (j.at("duration").is_string()) {
			t.duration = std::stoi(j.at("duration").get<std::string>());
		} else {
			j.at("duration").get_to(t.duration);
		}
		if (j.contains("artist_id")) {
			if (j.at("artist_id").is_string()) {
				t.artistID = std::stoi(j.at("artist_id").get<std::string>());
			} else {
				j.at("artist_id").get_to(t.artistID);
			}
		} else {
			t.artistID = -1;
		}

		if (j.contains("album_id")) {
			if (j.at("album_id").is_string()) {
				t.albumID = std::stoi(j.at("album_id").get<std::string>());
			} else {
				j.at("album_id").get_to(t.albumID);
			}
		} else {
			t.albumID = -1;
		}

		if (j.at("audio").is_string()) {
			t.audioURL = j.at("audio").get<std::string>();
		} else {
			j.at("audio").get_to(t.audioURL);
		}

		j.at("name").get_to(t.title);

		if (j.contains("image")) {
			j.at("image").get_to(t.imageURL);
		}
		else {
			t.imageURL = "";
		}

		if (j.contains("artist_name")) {
			j.at("artist_name").get_to(t.artistName);
		}
		else {
			t.artistName = "";
		}

		if (j.contains("album_name")) {
			j.at("album_name").get_to(t.albumName);
		}
		else {
			t.albumName = "";
		}

		if (j.contains("album_image")) {
			j.at("album_image").get_to(t.albumImageURL);
		}
		else {
			t.albumImageURL = "";
		}

		if (j.contains("releasedate")) {
			j.at("releasedate").get_to(t.releaseDate);
		}
		else {
			t.releaseDate = "";
		}
	}
}
