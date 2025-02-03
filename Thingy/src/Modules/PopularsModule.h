#pragma once
#include "Core\Module.h"
#include "Core\Track.h"
#include "Core\Album.h"
#include "Core\Artist.h"
#include "Core\NetworkManager.h"
#include "Core\Image.h"

namespace Thingy {
	class PopularsModule : public Module {
	public:

		PopularsModule(std::unique_ptr<NetworkManager>& networkManager, SDL_Renderer* renderer) : m_NetworkManager(networkManager), m_Renderer(renderer){
		};
		void OnLoad() override;
		void OnUpdate() override;
		void Window(std::string title) override;

		void OnRender() override;


		int MinWidth() const override { return 500; }
		int MaxWidth() const override { return 1280; }

		void GetPopulars();
		SDL_Texture* GetTexture(std::string& url);
		
		bool LoadTextureFromMemory(const void* data, size_t data_size, SDL_Renderer* renderer, SDL_Texture** out_texture, int* out_width, int* out_height) {
			int image_width = 0;
			int image_height = 0;
			int channels = 4;
			unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
			if (image_data == nullptr) {
				fprintf(stderr, "Failed to load image: %s\n", stbi_failure_reason());
				return false;
			}

			SDL_Surface* surface = SDL_CreateSurfaceFrom(image_width, image_height, SDL_PIXELFORMAT_RGBA32, (void*)image_data, channels * image_width);
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
			size_t file_size = (size_t)ftell(f);
			if (file_size == -1)
				return false;
			fseek(f, 0, SEEK_SET);
			void* file_data = IM_ALLOC(file_size);
			fread(file_data, 1, file_size, f);
			bool ret = LoadTextureFromMemory(file_data, file_size, renderer, out_texture, out_width, out_height);
			IM_FREE(file_data);
			return ret;
		}
		
		SDL_Texture* GetDefaultArtistImage();



		MODULE_CLASS_NAME("PopularsModule")

	private:
		struct SDL_TDeleter { void operator()(SDL_Texture* p) { SDL_DestroyTexture(p); } };
		
		SDL_Renderer* m_Renderer = nullptr;
		std::unique_ptr<NetworkManager>& m_NetworkManager;
		std::unordered_map<uint32_t, std::unique_ptr<SDL_Texture, SDL_TDeleter>> textures;
		std::vector<Track> weeklyTracks;
		std::vector<Track> monthlyTracks;
		std::vector<Album> weeklyAlbums; 
		std::vector<Album> monthlyAlbums;
		std::vector<Artist> weeklyArtists; 
		std::vector<Artist> monthlyArtists;
	};
}