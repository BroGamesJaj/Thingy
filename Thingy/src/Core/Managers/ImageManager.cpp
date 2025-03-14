#include "tpch.h"

#include "ImageManager.h"

namespace Thingy {

	Image ImageManager::GetImage(std::string& url) {
		std::vector<unsigned char> buffer;
		m_NetworkManager.DownloadImage(url, buffer);
		Image image(buffer);
		return image;
	}

	SDL_Texture* ImageManager::GetTextureFromImage(Image image) {
		return image.createTexture(m_Renderer);
	}

	SDL_Texture* ImageManager::GetTexture(std::string url) {
		std::vector<unsigned char> buffer;
		m_NetworkManager.DownloadImage(url, buffer);
		Image image(buffer);
		return image.createTexture(m_Renderer);
	}

	SDL_Surface* ImageManager::GetSurface(std::string& url) {
		std::vector<unsigned char> buffer;
		m_NetworkManager.DownloadImage(url, buffer);
		Image image(buffer);
		return image.createSurface();
	}
	 
	SDL_Texture* ImageManager::GetTextureFromSurface(SDL_Surface* surface) {
		SDL_Texture* texture = SDL_CreateTextureFromSurface(m_Renderer, surface);
		if (!texture) throw std::runtime_error("Failed to create texture: " + std::string(SDL_GetError()));
		SDL_DestroySurface(surface);

		return texture;
	}

	SDL_Texture* ImageManager::GetTextureFromFile(const char* filePath) {
		SDL_Texture* texture;
		int w, h;
		LoadTextureFromFile(filePath, m_Renderer, &texture, &w, &h);
		return texture;
	}

	SDL_Texture* ImageManager::GetDefaultArtistImage() {

		SDL_Texture* texture;
		int w, h;
		LoadTextureFromFile("../assets/images/defaultArtist.png", m_Renderer, &texture, &w, &h);
		return texture;
	}

	SDL_Texture* ImageManager::GetDefaultPlaylistImage() {

		SDL_Texture* texture;
		int w, h;
		LoadTextureFromFile("../assets/images/playlist_cover.png", m_Renderer, &texture, &w, &h);
		return texture;
	}
}