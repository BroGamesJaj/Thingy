#pragma once
#include "tpch.h"
#include "SDL3\SDL.h"
#include "stb/stb_image.h"

namespace Thingy {
    class Image {
    public:
        Image(const std::vector<unsigned char>& imageData) {
            int channels;
            pixels.reset(stbi_load_from_memory(imageData.data(), imageData.size(), &width, &height, &channels, 4));
            if (!pixels) throw std::runtime_error("Failed to decode image");
        }

        SDL_Texture* createTexture(SDL_Renderer* renderer) {
            SDL_Surface* surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, pixels.get(), width * 4);
            if (!surface) throw std::runtime_error("Failed to create surface: " + std::string(SDL_GetError()));

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);

            if (!texture) throw std::runtime_error("Failed to create texture: " + std::string(SDL_GetError()));

            return texture;
        }

		

    private:
        struct STB_Deleter { void operator()(unsigned char* p) { stbi_image_free(p); } };

        int width, height;
        std::unique_ptr<unsigned char, STB_Deleter> pixels;
    };
}