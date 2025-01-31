#pragma once
#include "tpch.h"
#include "Album.h"
#include <nlohmann\json.hpp>

using json = nlohmann::json;


namespace Thingy {
	struct Artist {
		int artistID = 0;
		std::string artistName = "";
		std::string artistWebsite = "";
		std::string artistJoinDate = "";
		std::string artistImageURL = "";
		std::vector<Album> albums;

        inline std::string toString() const {
            std::ostringstream oss;
            oss << "Artist Details:\n"
                << "ID: " << artistID << "\n"
                << "Name: " << artistName << "\n"
                << "Website: " << artistWebsite << "\n"
                << "Join Date: " << artistJoinDate << "\n"
                << "Image URL: " << artistImageURL << "\n"
                << "Albums: " << albums.size() << " albums";
            return oss.str();
        }
    };

    inline void to_json(json& j, const Artist& a) {
        j = json{
            {"artist_id", a.artistID},
            {"artist_name", a.artistName},
            {"artist_website", a.artistWebsite},
            {"artist_join_date", a.artistJoinDate},
            {"artist_image_url", a.artistImageURL}
        };
    }

    inline void from_json(const json& j, Artist& a) {
        if (j.at("id").is_string()) {
            a.artistID = std::stoi(j.at("id").get<std::string>());
        }
        else {
            j.at("id").get_to(a.artistID);
        }

        j.at("name").get_to(a.artistName);
        j.at("website").get_to(a.artistWebsite);
        j.at("joindate").get_to(a.artistJoinDate);
        j.at("image").get_to(a.artistImageURL);
    }
	
}