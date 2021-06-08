//
// Created by daholland on 4/28/21.
//

#ifndef GIBA_CONFIG_H
#define GIBA_CONFIG_H

#include <SDL.h>

struct ControlsConfig {
    SDL_KeyCode Up = SDLK_UP;
    SDL_KeyCode Down = SDLK_DOWN;
    SDL_KeyCode Left = SDLK_LEFT;
    SDL_KeyCode Right = SDLK_RIGHT;
    SDL_KeyCode Button_A = SDLK_x;
    SDL_KeyCode Button_B = SDLK_z;
    SDL_KeyCode Button_Start = SDLK_RETURN;
    SDL_KeyCode Button_Select = SDLK_RSHIFT;

    SDL_KeyCode Power_Toggle = SDLK_F8;
    SDL_KeyCode State_Save = SDLK_F9;
    SDL_KeyCode State_Load = SDLK_F10;

    SDL_KeyCode ToggleGui = SDLK_F12;
    SDL_KeyCode Quit = SDLK_q;
};

class AppConfig {
public:
    ControlsConfig Controls;
};


#endif //GIBA_CONFIG_H
