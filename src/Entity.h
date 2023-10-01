#pragma once

#include "SDL.h"
#include "BoxCollider.h"
#include "Vec2Int.h"

class Entity {
public:
    Vec2Int pos;
    BoxCollider collider;
    SDL_Texture* texture;
    double timeSinceCreationMs = 0.0;

    void Update(double frameDelta) {
        timeSinceCreationMs += frameDelta;
    }

    void Draw(SDL_Renderer* renderer) {
        SDL_Rect rect {static_cast<int>(collider.pos.x), static_cast<int>(collider.pos.y), static_cast<int>(collider.bounds.x), static_cast<int>(collider.bounds.y)};
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }

    Entity(Vec2Int pos, const Vec2& size, SDL_Texture* texture) : pos(pos), texture(texture) {
        collider = BoxCollider(static_cast<double>(pos.x), static_cast<double>(pos.y), size.x, size.y);
    }
};