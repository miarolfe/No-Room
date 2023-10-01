#pragma once

#include "SDL.h"
#include "Vec2.h"

class BoxCollider {
public:
    Vec2 pos {0.0, 0.0};
    Vec2 bounds {0.0, 0.0};

public:
    BoxCollider(double x, double y, double w, double h) {
        pos = {x, y};
        bounds = {w, h};
    }

    BoxCollider(Vec2 pos, Vec2 bounds) : pos(pos), bounds(bounds) {}

    BoxCollider(SDL_Rect rect) {
        pos = {static_cast<double>(rect.x), static_cast<double>(rect.y)};
        bounds = {static_cast<double>(rect.w), static_cast<double>(rect.h)};
    }

    bool Contains(Vec2 point) const {
        return (point.x > pos.x && point.x < pos.x + bounds.x && point.y > pos.y && point.y < pos.y + bounds.y);
    }
};