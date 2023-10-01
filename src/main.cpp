#define SDL_MAIN_HANDLED

#include <iostream>
#include <fstream>
#include <vector>
#include "json.hpp"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include "BoxCollider.h"
#include "Enemy.h"
#include "Entity.h"
#include "FrameTimer.h"
#include "InputHandler.h"
#include "Vec2.h"
#include "Vec2Int.h"

using string = std::string;
using json = nlohmann::json;

const SDL_Color WHITE = {255, 255, 255, 255};
const int TURRET_VALUE = 5;
const int OBSTACLE_VALUE = 1;

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

void DrawTextStringToWidth(const string& text, TTF_Font* font, Vec2Int pos, int width, SDL_Renderer* renderer) {
    Vec2Int requestedSize {0, 0};
    TTF_SizeUTF8(font, text.c_str(), &requestedSize.x, &requestedSize.y);
    const double ratio = static_cast<double>(requestedSize.x) / static_cast<double>(requestedSize.y);
    const int height = static_cast<int>(static_cast<float>(width) / ratio);
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), WHITE);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    const SDL_Rect rect {pos.x, pos.y, width, height};
    SDL_RenderCopy(renderer, textTexture, nullptr, &rect);
    SDL_DestroyTexture(textTexture);
}

void DrawTextStringToHeight(const string& text, TTF_Font* font, Vec2Int pos, int height, SDL_Renderer* renderer) {
    Vec2Int requestedSize {0, 0};
    TTF_SizeUTF8(font, text.c_str(), &requestedSize.x, &requestedSize.y);
    const double ratio = static_cast<double>(requestedSize.y) / static_cast<double>(requestedSize.x);
    const int width = static_cast<int>(static_cast<float>(height) / ratio);
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), WHITE);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    const SDL_Rect rect {pos.x, pos.y, width, height};
    SDL_RenderCopy(renderer, textTexture, nullptr, &rect);
    SDL_DestroyTexture(textTexture);
}

double Lerp(float start, float end, float t) {
    return start + t * (end - start);
}

void SpawnEnemy(std::vector<Enemy>& enemies, const Vec2& pos, SDL_Texture* texture, const Vec2& size, double speed) {
    enemies.emplace_back(texture, BoxCollider({pos.x, pos.y}, {size.x, size.y}), speed);
}

enum GroundType {
    DEFAULT_GROUND,
    SAFE_ZONE,
    WALL,
};

enum EntityType {
    NO_ENTITY,
    TURRET,
    OBSTACLE
};

enum EnemyType {
    VAN,
    PICKUP
};

struct EnemySpawn {
    double spawnTime;
    EnemyType type;
    double startY;
};

struct Cell {
    GroundType ground;
    EntityType entityType;
    Entity* entity = nullptr;
};

int main()
{
    double gameClock = 0.0f;
    bool gameplayActive = false;
    EntityType currentEntityType = NO_ENTITY;
    int balance = 100;
    const int GRID_WIDTH = 32;
    const int GRID_HEIGHT = 18;
    const Vec2Int boxSize {50, 50};
    Cell map[GRID_WIDTH][GRID_HEIGHT];

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            map[i][j].ground = WALL;
            map[i][j].entityType = NO_ENTITY;
            map[i][j].entity = nullptr;
        }
    }

    for (int i = 4; i < GRID_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            map[i][j].ground = DEFAULT_GROUND;
            map[i][j].entityType = NO_ENTITY;
            map[i][j].entity = nullptr;
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
    TTF_Init();
    // Mix_Init()

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

    string obstacle1Path = GetAssetFolderPath();
    obstacle1Path += "Obstacle1.png";
    SDL_Texture* obstacle1Texture = IMG_LoadTexture(renderer, obstacle1Path.c_str());

    string vanPath = GetAssetFolderPath();
    vanPath += "Van.png";
    SDL_Texture* vanTexture = IMG_LoadTexture(renderer, vanPath.c_str());

    string pickupTruckPath = GetAssetFolderPath();
    pickupTruckPath += "PickupTruck.png";
    SDL_Texture* pickupTruckTexture = IMG_LoadTexture(renderer, pickupTruckPath.c_str());

    string playButtonPath = GetAssetFolderPath();
    playButtonPath += "PlayButton.png";
    SDL_Texture* playButtonTexture = IMG_LoadTexture(renderer, playButtonPath.c_str());

    string pauseButtonPath = GetAssetFolderPath();
    pauseButtonPath += "PauseButton.png";
    SDL_Texture* pauseButtonTexture = IMG_LoadTexture(renderer, pauseButtonPath.c_str());

    string boldFontPath = GetAssetFolderPath();
    boldFontPath += "Changa-Bold.ttf";
    TTF_Font* boldFont = TTF_OpenFont(boldFontPath.c_str(), 120);

    string mediumFontPath = GetAssetFolderPath();
    mediumFontPath += "Changa-Medium.ttf";
    TTF_Font* mediumFont = TTF_OpenFont(mediumFontPath.c_str(), 120);

    string regularFontPath = GetAssetFolderPath();
    regularFontPath += "Changa-Regular.ttf";
    TTF_Font* regularFont = TTF_OpenFont(regularFontPath.c_str(), 120);

    string semiBoldFontPath = GetAssetFolderPath();
    semiBoldFontPath += "Changa-SemiBold.ttf";
    TTF_Font* semiBoldFont = TTF_OpenFont(semiBoldFontPath.c_str(), 120);

    FrameTimer frameTimer;
    InputHandler inputHandler;

    std::vector<Enemy> enemies;

    SDL_Rect turretButtonRect {25, 100, boxSize.x * 3, boxSize.y * 3};
    BoxCollider turretButtonCollider(turretButtonRect);
    SDL_Rect turretButtonImgRect {turretButtonRect.x + 15, turretButtonRect.y + 15, turretButtonRect.w - 30, turretButtonRect.h - 30};

    SDL_Rect obstacleButtonRect {25, turretButtonRect.y + turretButtonRect.h + 10, boxSize.x * 3, boxSize.y * 3};
    BoxCollider obstacleButtonCollider(obstacleButtonRect);
    SDL_Rect obstacleButtonImgRect {obstacleButtonRect.x + 15, obstacleButtonRect.y + 15, obstacleButtonRect.w - 35, obstacleButtonRect.h - 30};

    SDL_Rect sellButtonRect {25, obstacleButtonRect.y + obstacleButtonRect.h + 10, boxSize.x * 3, boxSize.y};
    BoxCollider sellButtonCollider(sellButtonRect);

    SDL_Rect vanButtonRect {25, sellButtonRect.y + sellButtonRect.h + 10, boxSize.x * 3, boxSize.y};
    BoxCollider vanButtonCollider(vanButtonRect);

    SDL_Rect pickupButtonRect {25, vanButtonRect.y + vanButtonRect.h + 10, boxSize.x * 3, boxSize.y};
    BoxCollider pickupButtonCollider(pickupButtonRect);

    SDL_Rect playButtonRect {25, pickupButtonRect.y + pickupButtonRect.h + 10, static_cast<int>(boxSize.x * 1.4), boxSize.y};
    BoxCollider playButtonCollider(playButtonRect);
    SDL_Rect playButtonImgRect {playButtonRect.x + 15, playButtonRect.y + 5, playButtonRect.w - 30, playButtonRect.h - 10};

    SDL_Rect pauseButtonRect {25 + playButtonRect.w + 10, pickupButtonRect.y + pickupButtonRect.h + 10, static_cast<int>(boxSize.x * 1.4), boxSize.y};
    BoxCollider pauseButtonCollider(pauseButtonRect);
    SDL_Rect pauseButtonImgRect {pauseButtonRect.x + 15, pauseButtonRect.y + 5, pauseButtonRect.w - 30, pauseButtonRect.h - 10};

    string enemySpawnPath = GetAssetFolderPath();
    enemySpawnPath += "game.json";
    std::ifstream enemySpawnFile(enemySpawnPath);
    json enemySpawnJson;
    enemySpawnFile >> enemySpawnJson;
    enemySpawnFile.close();

    std::vector<EnemySpawn> enemySpawns;

    for (const auto& item : enemySpawnJson) {
        bool validEnemySpawn = true;

        EnemySpawn enemySpawn{};
        enemySpawn.spawnTime = item["spawn_time"];

        string type = item["type"];

        if (type == "VAN") {
            enemySpawn.type = VAN;
        } else if (type == "PICKUP") {
            enemySpawn.type = PICKUP;
        } else {
            validEnemySpawn = false;
        }

        enemySpawn.startY = item["y"];

        if (validEnemySpawn) {
            enemySpawns.push_back(enemySpawn);
        }
    }

    std::sort(enemySpawns.begin(), enemySpawns.end(), [](const EnemySpawn& a, const EnemySpawn& b) {
       return a.spawnTime < b.spawnTime;
    });

    while (!inputHandler.state.exit) {
        inputHandler.Update();
        frameTimer.Update();

        if (gameplayActive) {
            gameClock += frameTimer.frameDeltaMs;
        }

        if (gameplayActive) {
            for (int i = 0; i < GRID_WIDTH; i++) {
                for (int j = 0; j < GRID_HEIGHT; j++) {
                    if (map[i][j].entityType != NO_ENTITY) {
                        map[i][j].entity->Update(frameTimer.frameDeltaMs);
                    }
                }
            }

            for (Enemy& enemy : enemies) {
                enemy.Update(frameTimer.frameDeltaMs);
            }
        }

        int currentCellX = inputHandler.state.mousePos.x / boxSize.x;
        int currentCellY = inputHandler.state.mousePos.y / boxSize.y;
        Cell& currentCell = map[currentCellX][currentCellY];

        if (vanButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)}) && inputHandler.state.leftMousePressedThisFrame) {
            SpawnEnemy(enemies, {-25.0, 25.0}, vanTexture, {static_cast<double>(boxSize.x * 2), static_cast<double>(boxSize.y)}, 0.25);
        }

        if (pickupButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)}) && inputHandler.state.leftMousePressedThisFrame) {
            SpawnEnemy(enemies, {-25.0, 25.0}, pickupTruckTexture, {static_cast<double>(boxSize.x * 2), static_cast<double>(boxSize.y)}, 0.25);
        }

        if (playButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)}) && inputHandler.state.leftMousePressedThisFrame) {
            gameplayActive = true;
        }

        if (pauseButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)}) && inputHandler.state.leftMousePressedThisFrame) {
            gameplayActive = false;
        }

        if (inputHandler.state.leftMousePressedThisFrame && currentCell.ground != WALL) {
            if (currentCell.entityType != NO_ENTITY) {
                switch (currentEntityType) {
                    case NO_ENTITY:
                        switch (currentCell.entityType) {
                            case TURRET:
                                balance += TURRET_VALUE;
                                delete currentCell.entity;
                                currentCell.entity = nullptr;
                                currentCell.entityType = NO_ENTITY;
                                break;
                            case OBSTACLE:
                                balance += OBSTACLE_VALUE;
                                delete currentCell.entity;
                                currentCell.entity = nullptr;
                                currentCell.entityType = NO_ENTITY;
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
            } else {
                switch (currentEntityType) {
                    case TURRET:
                        if (balance >= TURRET_VALUE) {
                            currentCell.entity = new Entity({currentCellX * boxSize.x, currentCellY * boxSize.y}, {static_cast<double>(boxSize.x), static_cast<double>(boxSize.y)}, turretTexture);
                            currentCell.entityType = TURRET;
                            balance -= TURRET_VALUE;
                        }
                        break;
                    case OBSTACLE:
                        if (balance >= OBSTACLE_VALUE) {
                            currentCell.entity = new Entity({currentCellX * boxSize.x, currentCellY * boxSize.y}, {static_cast<double>(boxSize.x), static_cast<double>(boxSize.y)}, obstacle1Texture);
                            currentCell.entityType = OBSTACLE;
                            balance -= OBSTACLE_VALUE;
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        SDL_SetRenderTarget(renderer, renderTexture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Rect currentlyHoveredCellRect {
                currentCellX * boxSize.x,
                currentCellY * boxSize.y,
                boxSize.x,
                boxSize.y
        };


        for (int i = 0; i < GRID_WIDTH; i++) {
            for (int j = 0; j < GRID_HEIGHT; j++) {
                SDL_Rect rect {i * boxSize.x, j * boxSize.y, boxSize.x, boxSize.y};

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
            }
        }

        if (currentCell.entityType == NO_ENTITY) {
            switch (currentEntityType) {
                case NO_ENTITY:
                    break;
                case TURRET:
                    SDL_RenderCopy(renderer, turretTexture, nullptr, &currentlyHoveredCellRect);
                    break;
                case OBSTACLE:
                    SDL_RenderCopy(renderer, obstacle1Texture, nullptr, &currentlyHoveredCellRect);
                    break;
            }
        }

        for (int i = 0; i < GRID_WIDTH; i++) {
            for (int j = 0; j < GRID_HEIGHT; j++) {
                if (map[i][j].entityType != NO_ENTITY) {
                    map[i][j].entity->Draw(renderer);
                }
            }
        }

        for (Enemy& enemy : enemies) {
            enemy.Draw(renderer);
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &currentlyHoveredCellRect);

        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
        SDL_Rect menuRect {0, 0, boxSize.x * 4, GRID_HEIGHT * boxSize.y};
        SDL_RenderFillRect(renderer, &menuRect);
        DrawTextStringToWidth("No Room", boldFont, {25, 10}, (boxSize.x * 4) - 50, renderer);

        string balanceStr = "$: " + std::to_string(balance);
        DrawTextStringToHeight(balanceStr, regularFont, {25, 50}, boxSize.y, renderer);

        if (turretButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)}) && inputHandler.state.leftMousePressed) {
            SDL_SetRenderDrawColor(renderer, 144, 144, 144, 255);
            currentEntityType = TURRET;
        } else if (turretButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)})) {
            SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
        }

        SDL_RenderFillRect(renderer, &turretButtonRect);
        SDL_RenderCopy(renderer, turretTexture, nullptr, &turretButtonImgRect);
        DrawTextStringToHeight("Turret", regularFont, {turretButtonRect.x + 5, turretButtonRect.y}, 30, renderer);
        DrawTextStringToWidth("$5", regularFont, {turretButtonRect.x + 5, turretButtonRect.y + turretButtonRect.h - 30}, 20, renderer);

        if (obstacleButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)}) && inputHandler.state.leftMousePressed) {
            SDL_SetRenderDrawColor(renderer, 144, 144, 144, 255);
            currentEntityType = OBSTACLE;
        } else if (obstacleButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)})) {
            SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
        }

        SDL_RenderFillRect(renderer, &obstacleButtonRect);
        SDL_RenderCopy(renderer, obstacle1Texture, nullptr, &obstacleButtonImgRect);
        DrawTextStringToHeight("Obstacle", regularFont, {obstacleButtonRect.x + 5, obstacleButtonRect.y}, 30, renderer);
        DrawTextStringToWidth("$1", regularFont, {obstacleButtonRect.x + 5, obstacleButtonRect.y + obstacleButtonRect.h - 35}, 20, renderer);

        if (sellButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)}) && inputHandler.state.leftMousePressed) {
            SDL_SetRenderDrawColor(renderer, 144, 144, 144, 255);
            currentEntityType = NO_ENTITY;
        } else if (sellButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)})) {
            SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
        }

        SDL_RenderFillRect(renderer, &sellButtonRect);
        DrawTextStringToHeight("Sell", regularFont, {sellButtonRect.x + 15, sellButtonRect.y}, sellButtonRect.h, renderer);

        if (vanButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)}) && inputHandler.state.leftMousePressed) {
            SDL_SetRenderDrawColor(renderer, 144, 144, 144, 255);
        } else if (vanButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)})) {
            SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
        }

        SDL_RenderFillRect(renderer, &vanButtonRect);
        DrawTextStringToHeight("Van", regularFont, {vanButtonRect.x + 15, vanButtonRect.y}, vanButtonRect.h, renderer);

        if (pickupButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)}) && inputHandler.state.leftMousePressed) {
            SDL_SetRenderDrawColor(renderer, 144, 144, 144, 255);
        } else if (pickupButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)})) {
            SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
        }

        SDL_RenderFillRect(renderer, &pickupButtonRect);
        DrawTextStringToHeight("Pickup", regularFont, {pickupButtonRect.x + 15, pickupButtonRect.y}, pickupButtonRect.h, renderer);

        if (playButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)}) && inputHandler.state.leftMousePressed) {
            SDL_SetRenderDrawColor(renderer, 144, 144, 144, 255);
        } else if (playButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)})) {
            SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
        }

        SDL_RenderFillRect(renderer, &playButtonRect);
        SDL_RenderCopy(renderer, playButtonTexture, nullptr, &playButtonImgRect);

        if (pauseButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)}) && inputHandler.state.leftMousePressed) {
            SDL_SetRenderDrawColor(renderer, 144, 144, 144, 255);
        } else if (pauseButtonCollider.Contains({static_cast<double>(inputHandler.state.mousePos.x), static_cast<double>(inputHandler.state.mousePos.y)})) {
            SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
        }

        SDL_RenderFillRect(renderer, &pauseButtonRect);
        SDL_RenderCopy(renderer, pauseButtonTexture, nullptr, &pauseButtonImgRect);

        SDL_SetRenderTarget(renderer, nullptr);

        if (aspectRatiosMatch) {
            SDL_RenderCopy(renderer, renderTexture, nullptr, nullptr);
        } else {
            // TODO: Letterbox / pillarbox
            SDL_RenderCopy(renderer, renderTexture, nullptr, nullptr);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(renderTexture);
    SDL_DestroyTexture(wall1Texture);
    SDL_DestroyTexture(floor1Texture);
    SDL_DestroyTexture(floor2Texture);
    SDL_DestroyTexture(turretTexture);
    SDL_DestroyTexture(obstacle1Texture);
    SDL_DestroyTexture(vanTexture);
    SDL_DestroyTexture(pickupTruckTexture);
    SDL_DestroyTexture(playButtonTexture);
    SDL_DestroyTexture(pauseButtonTexture);
    TTF_CloseFont(boldFont);
    TTF_CloseFont(mediumFont);
    TTF_CloseFont(regularFont);
    TTF_CloseFont(semiBoldFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}