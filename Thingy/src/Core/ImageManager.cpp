#include "tpch.h"

#include "ImageManager.h"

namespace Thingy {

	SDL_Texture* ImageManager::GetTexture(std::string& url) {
		std::vector<unsigned char> buffer;
		m_NetworkManager->DownloadImage(url, buffer);
		Image image(buffer);
		SDL_Texture* texture = image.createTexture(m_Renderer);
		return texture;
	}

	SDL_Texture* ImageManager::GetDefaultArtistImage() {

		SDL_Texture* texture;
		int w, h;
		LoadTextureFromFile("../assets/images/defaultArtist.png", m_Renderer, &texture, &w, &h);
		return texture;
	}
}