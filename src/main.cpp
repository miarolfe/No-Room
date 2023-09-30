#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL.h>
#include "FrameTimer.h"
#include "InputHandler.h"
#include "Vec2.h"
#include "Vec2Int.h"

// Window size
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

int main(int argc, char* args[])
{
    // SDL initialisation
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("SDL_Init Error: %s\n", SDL_GetError());
        return -1;
    }

    // Window creation and position in the center of the screen
    SDL_Window* window = SDL_CreateWindow("Hello World SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        SDL_Log("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return -1;
    }

    // Render creation
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return -1;
    }

    const double MOVE_SPEED = 1.0;

    FrameTimer frameTimer;
    InputHandler inputHandler;
    Vec2 boxPos {0.0, 0.0};
    Vec2 boxSize {50.0, 50.0};

    SDL_Rect boxRect = {
            static_cast<int>(boxPos.x),
            static_cast<int>(boxPos.y),
            static_cast<int>(boxSize.x),
            static_cast<int>(boxSize.y),
    };

    while (!inputHandler.state.exit) {
        frameTimer.Update();
        inputHandler.Update();

        if (inputHandler.state.leftKeyPressed  || inputHandler.state.aKeyPressed) boxPos.x -= (MOVE_SPEED * frameTimer.frameDeltaMs);
        if (inputHandler.state.rightKeyPressed || inputHandler.state.dKeyPressed) boxPos.x += (MOVE_SPEED * frameTimer.frameDeltaMs);
        if (inputHandler.state.upKeyPressed    || inputHandler.state.wKeyPressed) boxPos.y -= (MOVE_SPEED * frameTimer.frameDeltaMs);
        if (inputHandler.state.downKeyPressed  || inputHandler.state.sKeyPressed) boxPos.y += (MOVE_SPEED * frameTimer.frameDeltaMs);

        boxRect.x = static_cast<int>(boxPos.x);
        boxRect.y = static_cast<int>(boxPos.y);
        boxRect.w = static_cast<int>(boxSize.x);
        boxRect.h = static_cast<int>(boxSize.y);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &boxRect);
        SDL_RenderPresent(renderer);
    }

    // Destroy the render, window and finalise SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}