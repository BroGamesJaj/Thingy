#pragma once
#include "tpch.h"
#include "Core\Image.h"
#include "NetworkManager.h"
#include <SDL3\SDL.h>
#include "imgui.h"
#include "imgui_internal.h"


namespace Thingy {
	struct SDL_TDeleter { void operator()(SDL_Texture* p) noexcept { SDL_DestroyTexture(p); } };

	class ImageManager {
	public:
		ImageManager(NetworkManager& networkManager, SDL_Renderer* renderer) noexcept : m_NetworkManager(networkManager), m_Renderer(renderer) {};
	
		ImageManager(const ImageManager&) = delete;
		void operator=(const ImageManager&) = delete;

		Image GetImage(std::string& url);

		SDL_Texture* GetTextureFromImage(Image image);

		SDL_Texture* GetTexture(std::string url);
		SDL_Surface* GetSurface(std::string& url);
		SDL_Texture* GetTextureFromSurface(SDL_Surface* surface);
		SDL_Texture* GetTextureFromFile(const char* filePath);

		bool LoadTextureFromMemory(const void* data, size_t data_size, SDL_Renderer* renderer, SDL_Texture** out_texture, int* out_width, int* out_height) noexcept {
			int image_width = 0;
			int image_height = 0;
			const int channels = 4;
			unsigned char* image_data = stbi_load_from_memory(static_cast<const unsigned char*>(data), static_cast<int>(data_size), &image_width, &image_height, NULL, 4);
			if (!image_data) {
				fprintf(stderr, "Failed to load image: %s\n", stbi_failure_reason());
				return false;
			}

			SDL_Surface* surface = SDL_CreateSurfaceFrom(image_width, image_height, SDL_PIXELFORMAT_RGBA32, static_cast<void*>(image_data), channels * image_width);
			if (surface == nullptr) {
				fprintf(stderr, "Failed to create SDL surface: %s\n", SDL_GetError());
				return false;
			}

			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			if (texture == nullptr)
				fprintf(stderr, "Failed to create SDL texture: %s\n", SDL_GetError());

			*out_texture = texture;
			*out_width = image_width;
			*out_height = image_height;

			SDL_DestroySurface(surface);
			stbi_image_free(image_data);

			return true;
		}

		bool LoadTextureFromFile(const char* file_name, SDL_Renderer* renderer, SDL_Texture** out_texture, int* out_width, int* out_height) {
			FILE* f = fopen(file_name, "rb");
			if (f == NULL)
				return false;
			fseek(f, 0, SEEK_END);
			const size_t file_size = static_cast<size_t>(ftell(f));
			if (file_size == -1)
				return false;
			fseek(f, 0, SEEK_SET);
			void* file_data = IM_ALLOC(file_size);
			fread(file_data, 1, file_size, f);
			const bool ret = LoadTextureFromMemory(file_data, file_size, renderer, out_texture, out_width, out_height);
			IM_FREE(file_data);
			return ret;
		}

		SDL_Texture* GetDefaultArtistImage();
		SDL_Texture* GetDefaultPlaylistImage();

		const bool HasTextureAt(const int& identifier) {
			return (textures[identifier] ? true : false);
		}
		const ImTextureID GetImTexture(const int& identifier) {
			return reinterpret_cast<ImTextureID>(textures[identifier].get());
		}

		void AddTexture(const int& identifier, SDL_Texture* texture) {
			textures[identifier] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(texture);
		}

		const bool HasPlaylistTextureAt(const int& identifier) {
			return (playlistTextures[identifier] ? true : false);
		}
		const ImTextureID GetPlaylistImTexture(const int& identifier) {
			return reinterpret_cast<ImTextureID>(playlistTextures[identifier].get());
		}

		void AddPlaylistTexture(const int& identifier, SDL_Texture* texture) {
			playlistTextures[identifier] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(texture);
		}
	private:
		NetworkManager& m_NetworkManager;
		SDL_Renderer* m_Renderer;

		std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>> textures;
		std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>> userTextures;
		std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>> playlistTextures;
	};
}