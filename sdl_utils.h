#ifndef SDL_UTILS_H
#define SDL_UTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define WIDTH 640
#define HEIGHT 480

// Declare variables as extern to avoid multiple definitions
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;

// Function prototypes
void initializeSDL();
void closeSDL();

#endif // SDL_UTILS_H
