#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_ttf.h>
#include "sdl_utils.h"
#include "shared.h"

#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 100
#define BALL_SIZE 20
#define SPEED 15
#define MS_PER_FRAME 16

int paddle1Y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
int paddle2Y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
int ballX = WIDTH / 2 - BALL_SIZE / 2;
int ballY = HEIGHT / 2 - BALL_SIZE / 2;
int ballXSpeed = SPEED;
int ballYSpeed = SPEED;

SDL_Thread* inputThread;
SDL_mutex* mutex = NULL;
bool restartGame = false;
bool gameEnded = false;
bool winnerDeclared = false;
int winner = 0;

int inputThreadFunction(void* data) {
    SDL_Event event;
    while (1) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(0);
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        if (paddle1Y > 0) {
                            paddle1Y -= SPEED;
                        }
                        break;
                    case SDLK_s:
                        if (paddle1Y < HEIGHT - PADDLE_HEIGHT) {
                            paddle1Y += SPEED;
                        }
                        break;
                    case SDLK_UP:
                        if (paddle2Y > 0) {
                            paddle2Y -= SPEED;
                        }
                        break;
                    case SDLK_DOWN:
                        if (paddle2Y < HEIGHT - PADDLE_HEIGHT) {
                            paddle2Y += SPEED;
                        }
                        break;
                    case SDLK_SPACE:
                        restartGame = true;
                        break;
                }
            }
        }
    }
    
    // This is a dummy return, as SDL_ThreadFunction expects an int return type.
    return 0;
}

void updateGame(float deltaTime) {
    SDL_LockMutex(mutex);

    if (gameEnded) {
        SDL_UnlockMutex(mutex);
        return;
    }

    // Move ball
    ballX += ballXSpeed * deltaTime;
    ballY += ballYSpeed * deltaTime;

    // Check if the ball is beyond the window boundaries
    if (ballX < 0) {
        // Player 2 wins
        printf("Player 2 wins!\n");
        gameEnded = true;
        winner = 2;
    } else if (ballX >= WIDTH) {
        // Player 1 wins
        printf("Player 1 wins!\n");
        gameEnded = true;
        winner = 1;
    }

    // Check collisions with top and bottom edges
    if (ballY <= 0 || ballY >= HEIGHT - BALL_SIZE) {
        ballYSpeed = -ballYSpeed;
    }

    // Check collisions with paddles
    if (ballX <= PADDLE_WIDTH && ballY >= paddle1Y && ballY <= paddle1Y + PADDLE_HEIGHT) {
        ballXSpeed = -ballXSpeed;
    }

    if (ballX >= WIDTH - PADDLE_WIDTH - BALL_SIZE && ballY >= paddle2Y && ballY <= paddle2Y + PADDLE_HEIGHT) {
        ballXSpeed = -ballXSpeed;
    }

    SDL_UnlockMutex(mutex);
}

void drawGame() {
    SDL_LockMutex(mutex);

    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw paddles
    SDL_Rect paddle1 = {0, paddle1Y, PADDLE_WIDTH, PADDLE_HEIGHT};
    SDL_Rect paddle2 = {WIDTH - PADDLE_WIDTH, paddle2Y, PADDLE_WIDTH, PADDLE_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddle1);
    SDL_RenderFillRect(renderer, &paddle2);

    // Draw ball
    SDL_Rect ball = {ballX, ballY, BALL_SIZE, BALL_SIZE};
    SDL_RenderFillRect(renderer, &ball);

    // Update the screen
    SDL_RenderPresent(renderer);

    SDL_UnlockMutex(mutex);
}

void drawWinner() {
    SDL_Color textColor = {255, 255, 255};
    SDL_Surface* surfaceMessage = NULL;
    SDL_Texture* message = NULL;
    char winnerText[50];

    if (winner == 1) {
        sprintf(winnerText, "Player 1 wins!");
    } else if (winner == 2) {
        sprintf(winnerText, "Player 2 wins!");
    }

    // Render text using loaded font
    surfaceMessage = TTF_RenderText_Solid(font, winnerText, textColor);
    message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect messageRect;
    messageRect.x = WIDTH / 2 - 100;
    messageRect.y = HEIGHT / 2 - 20;
    messageRect.w = 200;
    messageRect.h = 40;

    SDL_RenderCopy(renderer, message, NULL, &messageRect);

    // Free resources
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}

int main() {
    // Initialize SDL
    initializeSDL();

    // Load font
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationMono-Bold.ttf", 24);
    if (!font) {
        printf("Unable to load font: %s\n", TTF_GetError());
        return 1; // Return an error code indicating font loading failure
    }

    // Create mutex for thread synchronization
    mutex = SDL_CreateMutex();

    // Create input handling thread
    inputThread = SDL_CreateThread(inputThreadFunction, "InputThread", NULL);

    Uint32 prevTicks = SDL_GetTicks(); // Time of the previous frame

    // Main game loop
    while (1) {
        Uint32 currentTicks = SDL_GetTicks(); // Time of the current frame
        Uint32 deltaTime = currentTicks - prevTicks; // Time elapsed since the last frame
        prevTicks = currentTicks;

        if (restartGame) {
            // Reset game state
            ballX = WIDTH / 2 - BALL_SIZE / 2;
            ballY = HEIGHT / 2 - BALL_SIZE / 2;
            paddle1Y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
            paddle2Y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
            restartGame = false;
            gameEnded = false;
            winnerDeclared = false;
        }

        updateGame(deltaTime / 1000.0); // Convert deltaTime to seconds
        drawGame();

        if (gameEnded && !winnerDeclared) {
            drawWinner();
            winnerDeclared = true;
        }

        if (gameEnded) {
            SDL_Delay(2000); // Delay for 2 seconds before restarting the game
            restartGame = true;
        }
    }

    // Clean up resources
    closeSDL();

    return 0;
}

