#pragma once
#include "tpch.h"

namespace Thingy {
	class Artist {
	public:
		Artist(int id, std::string name, std::string website, std::string joinDate, std::string imageURL) 
			: artistID(id), artistName(name), artistWebsite(website), artistJoinDate(joinDate), artistImageURL(imageURL)
		{}


	private:
		int artistID;
		std::string artistName;
		std::string artistWebsite;
		std::string artistJoinDate;
		std::string artistImageURL;

	};

	
}