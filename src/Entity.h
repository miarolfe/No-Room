#pragma once

#include "SDL.h"
#include "BoxCollider.h"
#include "Vec2Int.h"

class Entity {
public:
    BoxCollider collider;
    SDL_Texture* texture;
    double timeSinceCreationMs = 0.0;

    virtual void Update(double frameDelta) {
        timeSinceCreationMs += frameDelta;
    }

    void Draw(SDL_Renderer* renderer) {
        SDL_Rect rect {static_cast<int>(collider.pos.x), static_cast<int>(collider.pos.y), static_cast<int>(collider.bounds.x), static_cast<int>(collider.bounds.y)};
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }

    Entity(Vec2 pos, const Vec2& size, SDL_Texture* texture) : texture(texture) {
        collider = BoxCollider(static_cast<double>(pos.x), static_cast<double>(pos.y), size.x, size.y);
    }
};

class TurretEntity : public Entity {
public:
    const double FIRE_INTERVAL_MS = 2000.0;
    double fireTimerMs = 0.0;

    std::vector<Entity>& projectiles;
    Vec2 projectileSize;
    SDL_Texture* projectileTexture;
    AudioHandler& audioHandler;

    TurretEntity(Vec2 pos, const Vec2& turretSize, SDL_Texture* turretTexture, const Vec2& projectileSize, SDL_Texture* projectileTexture, std::vector<Entity>& projectiles, AudioHandler& audioHandler) : Entity(pos, turretSize, turretTexture),
                                                                                                                                                                                  projectileSize(projectileSize), projectileTexture(projectileTexture), projectiles(projectiles), audioHandler(audioHandler) {
    }

    void Update(double frameDelta) override {
        timeSinceCreationMs += frameDelta;
        fireTimerMs += frameDelta;

        if (fireTimerMs >= FIRE_INTERVAL_MS) {
            Fire();
            fireTimerMs = 0.0;
        }
    }

    void Fire() {
        Vec2 projectilePos {collider.pos.x + (collider.bounds.x/2) - (projectileSize.x/2), collider.pos.y + (collider.bounds.y/2) - (projectileSize.y/2)};
        Entity projectile(projectilePos, projectileSize, projectileTexture);
        projectiles.push_back(projectile);
        audioHandler.PlayEffect("TurretFire");
    }
};