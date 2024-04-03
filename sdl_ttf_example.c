#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "sdl_utils.h"
#include "shared.h"

#define WIDTH 640
#define HEIGHT 480

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Create SDL window and renderer
    window = SDL_CreateWindow("SDL TTF Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (window == NULL || renderer == NULL) {
        fprintf(stderr, "Unable to create window or renderer: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() != 0) {
        fprintf(stderr, "Unable to initialize SDL_ttf: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Load font
    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
    if (font == NULL) {
        fprintf(stderr, "Unable to load font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Font loaded successfully
    printf("Font loaded successfully!\n");

    // Main loop
    SDL_Event event;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw text
        SDL_Color color = {255, 255, 255, 255};
        SDL_Surface* surface = TTF_RenderText_Solid(font, "Hello, SDL!", color);
        if (surface == NULL) {
            fprintf(stderr, "Unable to render text: %s\n", TTF_GetError());
            break;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture == NULL) {
            fprintf(stderr, "Unable to create texture: %s\n", SDL_GetError());
            SDL_FreeSurface(surface);
            break;
        }

        SDL_Rect destRect = {WIDTH / 2 - surface->w / 2, HEIGHT / 2 - surface->h / 2, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &destRect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        // Update the screen
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

