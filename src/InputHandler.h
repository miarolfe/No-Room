#pragma once

#include "SDL.h"

class InputHandler {
public:
    struct State {
        bool exit = false;
        bool leftMousePressed = false;
        bool rightMousePressed = false;
        bool wKeyPressed = false;
        bool aKeyPressed = false;
        bool sKeyPressed = false;
        bool dKeyPressed = false;
        bool upKeyPressed = false;
        bool downKeyPressed = false;
        bool leftKeyPressed = false;
        bool rightKeyPressed = false;
    };

    State state;

private:
    SDL_Event eventData;

public:
    InputHandler() = default;

    void Update() {
        while (SDL_PollEvent(&eventData))
        {
            switch (eventData.type)
            {
                case SDL_QUIT:
                    state.exit = true;
                    break;

                case SDL_KEYDOWN:
                    switch (eventData.key.keysym.sym) {
                        case SDLK_w:
                            state.wKeyPressed = true;
                            break;
                        case SDLK_a:
                            state.aKeyPressed = true;
                            break;
                        case SDLK_s:
                            state.sKeyPressed = true;
                            break;
                        case SDLK_d:
                            state.dKeyPressed = true;
                            break;
                        case SDLK_UP:
                            state.upKeyPressed = true;
                            break;
                        case SDLK_LEFT:
                            state.leftKeyPressed = true;
                            break;
                        case SDLK_DOWN:
                            state.downKeyPressed = true;
                            break;
                        case SDLK_RIGHT:
                            state.rightKeyPressed = true;
                            break;
                        default:
                            break;
                    }
                    break;

                case SDL_KEYUP:
                    switch (eventData.key.keysym.sym) {
                        case SDLK_w:
                            state.wKeyPressed = false;
                            break;
                        case SDLK_a:
                            state.aKeyPressed = false;
                            break;
                        case SDLK_s:
                            state.sKeyPressed = false;
                            break;
                        case SDLK_d:
                            state.dKeyPressed = false;
                            break;
                        case SDLK_UP:
                            state.upKeyPressed = false;
                            break;
                        case SDLK_LEFT:
                            state.leftKeyPressed = false;
                            break;
                        case SDLK_DOWN:
                            state.downKeyPressed = false;
                            break;
                        case SDLK_RIGHT:
                            state.rightKeyPressed = false;
                            break;
                        default:
                            break;
                    }
                    break;
            }
        }
    }
};