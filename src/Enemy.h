#pragma once

#include "SDL.h"
#include "BoxCollider.h"

class Enemy {
public:
    int id;
    BoxCollider collider;
    SDL_Texture* texture;
    double speed;

    Enemy(int id, SDL_Texture* texture, BoxCollider collider, double speed) : id(id), collider(collider), texture(texture), speed(speed) {}

    void Update(double frameDelta) {
        collider.pos.x += (frameDelta * speed);
    }

    void Draw(SDL_Renderer* renderer) {
        SDL_Rect rect {static_cast<int>(collider.pos.x), static_cast<int>(collider.pos.y), static_cast<int>(collider.bounds.x), static_cast<int>(collider.bounds.y)};
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }

    bool operator== (const Enemy& other) const {
        return (id == other.id);
    }
};
