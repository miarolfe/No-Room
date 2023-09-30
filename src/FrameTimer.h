#pragma once

#include "SDL.h"

class FrameTimer {
public:
    double frameDeltaMs = 1e-06;

private:
    double oldTimeMs = 0.0;
    double newTimeMs = 1.0;

public:
    FrameTimer() = default;

    void Update() {
        oldTimeMs = newTimeMs;
        newTimeMs = static_cast<double>(SDL_GetTicks64());
        frameDeltaMs = newTimeMs - oldTimeMs;
    }
};
