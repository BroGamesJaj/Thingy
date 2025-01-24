#pragma once
#include "tpch.h"
#include "Artist.h"
#include <nlohmann\json.hpp>

using json = nlohmann::json;

namespace Thingy {

	struct Track {
		int id = 0;
		std::string title = "";
		int duration = 0;
		std::string imageURL = "";
		int artistID = 0;
		std::string artistName = "";
		int albumID = 0;
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

		if (j.at("artist_id").is_string()) {
			t.artistID = std::stoi(j.at("artist_id").get<std::string>());
		} else {
			j.at("artist_id").get_to(t.artistID);
		}

		if (j.at("album_id").is_string()) {
			t.albumID = std::stoi(j.at("album_id").get<std::string>());
		} else {
			j.at("album_id").get_to(t.albumID);
		}

		if (j.at("audio").is_string()) {
			t.audioURL = j.at("audio").get<std::string>();
		} else {
			j.at("audio").get_to(t.audioURL);
		}

		j.at("name").get_to(t.title);
		j.at("image").get_to(t.imageURL);
		j.at("artist_name").get_to(t.artistName);
		j.at("album_name").get_to(t.albumName);
		j.at("album_image").get_to(t.albumImageURL);
		j.at("releasedate").get_to(t.releaseDate);
	}
}
