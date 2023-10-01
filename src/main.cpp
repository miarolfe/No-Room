#define SDL_MAIN_HANDLED

#include <iostream>
#include "SDL.h"
#include "SDL_image.h"
#include "FrameTimer.h"
#include "InputHandler.h"
#include "Vec2.h"
#include "Vec2Int.h"

using string = std::string;

const char* GetAssetFolderPath() {
    const char* platform = SDL_GetPlatform();

    if (strcmp(platform, "Windows") == 0) {
        return "assets/";
    } else if (strcmp(platform, "Mac OS X") == 0) {
        return "../Resources/";
    }
}

double Lerp(float start, float end, float t) {
    return start + t * (end - start);
}

int main()
{
    const int TARGET_WIDTH = 1600;
    const int TARGET_HEIGHT = 900;
    const double TARGET_ASPECT_RATIO = static_cast<double>(TARGET_WIDTH) / static_cast<double>(TARGET_HEIGHT);

    const int WINDOW_WIDTH = 1600;
    const int WINDOW_HEIGHT = 900;
    const double WINDOW_ASPECT_RATIO = static_cast<double>(WINDOW_WIDTH) / static_cast<double>(WINDOW_HEIGHT);

    bool aspectRatiosMatch = true;

    if (TARGET_ASPECT_RATIO != WINDOW_ASPECT_RATIO) {
        aspectRatiosMatch = false;
    }

    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);

    // Window creation and position in the center of the screen
    SDL_Window* window = SDL_CreateWindow("LD54", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    // Render creation
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Texture* renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, TARGET_WIDTH, TARGET_HEIGHT);

    string wall1Path = GetAssetFolderPath();
    wall1Path += "Wall1.png";
    SDL_Texture* wall1Texture = IMG_LoadTexture(renderer, wall1Path.c_str());

    const double MOVE_SPEED = 1.0;

    FrameTimer frameTimer;
    InputHandler inputHandler;
    Vec2 boxPos {25.0, 25.0};
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

        // boxPos = {static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)};

        if (boxPos.x < 0.0) boxPos.x = 0.0;
        if (boxPos.x > TARGET_WIDTH - boxSize.x) boxPos.x = TARGET_WIDTH - boxSize.x;
        if (boxPos.y < 0.0) boxPos.y = 0.0;
        if (boxPos.y > TARGET_HEIGHT - boxSize.y) boxPos.y = TARGET_HEIGHT - boxSize.y;

        boxRect.x = static_cast<int>(boxPos.x);
        boxRect.y = static_cast<int>(boxPos.y);
        boxRect.w = static_cast<int>(boxSize.x);
        boxRect.h = static_cast<int>(boxSize.y);

        SDL_SetRenderTarget(renderer, renderTexture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int i = 0; i < TARGET_WIDTH; i += static_cast<int>(boxSize.x)) {
            for (int j = 0; j < TARGET_HEIGHT; j += static_cast<int>(boxSize.y)) {
                SDL_Rect rect {i, j, static_cast<int>(boxSize.x), static_cast<int>(boxSize.y)};
                SDL_RenderDrawRect(renderer, &rect);
            }
        }

        int s = TARGET_HEIGHT - (static_cast<int>(boxSize.y) * 2);
        for (int i = 0; i < TARGET_WIDTH; i += static_cast<int>(boxSize.x)) {
            SDL_Rect rect {i, s, static_cast<int>(boxSize.x), static_cast<int>(boxSize.y)};
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 128);
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &boxRect);

        SDL_Rect tmp {0, 0, 50, 50};
        SDL_RenderCopy(renderer, wall1Texture, nullptr, &tmp);

        SDL_SetRenderTarget(renderer, nullptr);

        if (aspectRatiosMatch) {
            SDL_RenderCopy(renderer, renderTexture, nullptr, nullptr);
        } else {
            // TODO: Letterbox / pillarbox
            SDL_RenderCopy(renderer, renderTexture, nullptr, nullptr);
        }

        SDL_RenderPresent(renderer);
    }

    // Destroy the render, window and finalise SDL
    SDL_DestroyTexture(renderTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}