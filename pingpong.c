#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

#define WIDTH 640
#define HEIGHT 480
#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 100
#define BALL_SIZE 20
#define SPEED 5

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_mutex* mutex = NULL;

int paddle1Y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
int paddle2Y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
int ballX = WIDTH / 2 - BALL_SIZE / 2;
int ballY = HEIGHT / 2 - BALL_SIZE / 2;
int ballXSpeed = SPEED;
int ballYSpeed = SPEED;

SDL_Thread* inputThread;

void updateGame() {
    SDL_LockMutex(mutex);

    // Move ball
    ballX += ballXSpeed;
    ballY += ballYSpeed;

    // Check collisions
    if (ballY <= 0 || ballY >= HEIGHT - BALL_SIZE) {
        ballYSpeed = -ballYSpeed;
    }

    if (ballX <= PADDLE_WIDTH && ballY >= paddle1Y && ballY <= paddle1Y + PADDLE_HEIGHT) {
        ballXSpeed = -ballXSpeed;
    }

    if (ballX >= WIDTH - PADDLE_WIDTH - BALL_SIZE && ballY >= paddle2Y && ballY <= paddle2Y + PADDLE_HEIGHT) {
        ballXSpeed = -ballXSpeed;
    }

    SDL_UnlockMutex(mutex);
}

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
                }
            }
        }
    }
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

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Create SDL window and renderer
    window = SDL_CreateWindow("Ping Pong Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (window == NULL || renderer == NULL) {
        fprintf(stderr, "Unable to create window or renderer: %s\n", SDL_GetError());
        return 1;
    }

    // Create mutex for thread synchronization
    mutex = SDL_CreateMutex();

    // Create input handling thread
    inputThread = SDL_CreateThread(inputThreadFunction, "InputThread", NULL);

    // Main game loop
    while (1) {
        updateGame();
        drawGame();
        SDL_Delay(16);  // Cap the frame rate to approximately 60 frames per second
    }

    // Clean up resources
    SDL_DestroyMutex(mutex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


// use this to run code
sudo apt-get update
sudo apt-get install libsdl2-dev
