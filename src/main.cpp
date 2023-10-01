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
    } else {
        return "";
    }
}

double Lerp(float start, float end, float t) {
    return start + t * (end - start);
}

enum GroundType {
    DEFAULT_GROUND,
    SAFE_ZONE,
    WALL,
};

enum EntityType {
    NO_ENTITY,
    TURRET,
};

struct Cell {
    GroundType ground;
    EntityType entity;
};

int main()
{
    const int GRID_WIDTH = 32;
    const int GRID_HEIGHT = 18;
    Cell map[GRID_WIDTH][GRID_HEIGHT];

    for (int i = 0; i < GRID_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            map[i][j].ground = DEFAULT_GROUND;
            map[i][j].entity = NO_ENTITY;
        }
    }

    for (int i = 0; i < GRID_HEIGHT; i++) {
        map[GRID_WIDTH-3][i].ground = WALL;
    }

    for (int i = GRID_WIDTH-2; i < GRID_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            map[i][j].ground = SAFE_ZONE;
        }
    }

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

    string floor1Path = GetAssetFolderPath();
    floor1Path += "Floor1.png";
    SDL_Texture* floor1Texture = IMG_LoadTexture(renderer, floor1Path.c_str());

    string floor2Path = GetAssetFolderPath();
    floor2Path += "Floor2.png";
    SDL_Texture* floor2Texture = IMG_LoadTexture(renderer, floor2Path.c_str());

    string turretPath = GetAssetFolderPath();
    turretPath += "Turret.png";
    SDL_Texture* turretTexture = IMG_LoadTexture(renderer, turretPath.c_str());

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

        int currentCellX = inputHandler.state.mousePos.x / static_cast<int>(boxSize.x);
        int currentCellY = inputHandler.state.mousePos.y / static_cast<int>(boxSize.y);
        Cell& currentCell = map[currentCellX][currentCellY];

        if (inputHandler.state.leftMousePressedThisFrame)  currentCell.entity = TURRET;
        if (inputHandler.state.rightMousePressedThisFrame) currentCell.entity = NO_ENTITY;
        if (inputHandler.state.leftKeyPressed  || inputHandler.state.aKeyPressed) boxPos.x -= (MOVE_SPEED * frameTimer.frameDeltaMs);
        if (inputHandler.state.rightKeyPressed || inputHandler.state.dKeyPressed) boxPos.x += (MOVE_SPEED * frameTimer.frameDeltaMs);
        if (inputHandler.state.upKeyPressed    || inputHandler.state.wKeyPressed) boxPos.y -= (MOVE_SPEED * frameTimer.frameDeltaMs);
        if (inputHandler.state.downKeyPressed  || inputHandler.state.sKeyPressed) boxPos.y += (MOVE_SPEED * frameTimer.frameDeltaMs);

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

        for (int i = 0; i < GRID_WIDTH; i++) {
            for (int j = 0; j < GRID_HEIGHT; j++) {
                SDL_Rect rect {i * static_cast<int>(boxSize.x), j * static_cast<int>(boxSize.y), static_cast<int>(boxSize.x), static_cast<int>(boxSize.y)};

                switch (map[i][j].ground) {
                    case DEFAULT_GROUND:
                        SDL_RenderCopy(renderer, floor1Texture, nullptr, &rect);
                        break;
                    case SAFE_ZONE:
                        SDL_RenderCopy(renderer, floor2Texture, nullptr, &rect);
                        break;
                    case WALL:
                        SDL_RenderCopy(renderer, wall1Texture, nullptr, &rect);
                        break;
                }

                switch (map[i][j].entity) {
                    case NO_ENTITY:
                        break;
                    case TURRET:
                        SDL_RenderCopy(renderer, turretTexture, nullptr, &rect);
                        break;
                }

            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &boxRect);

        SDL_Rect currentlyHoveredCellRect {
                currentCellX * static_cast<int>(boxSize.x),
                currentCellY * static_cast<int>(boxSize.y),
                static_cast<int>(boxSize.x),
                static_cast<int>(boxSize.y)
        };

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &currentlyHoveredCellRect);

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