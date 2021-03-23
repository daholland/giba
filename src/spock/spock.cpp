//
// Created by daholland on 3/23/21.
//

#include "spock.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <vk_types.h>
#include <vk_init.h>

void Spock::init() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

    _window = SDL_CreateWindow(
                "spock gibalib",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                _windowExtent.width,
                _windowExtent.height,
                window_flags
            );

    _isInitialized = true;
}

void Spock::cleanup() {
    if (_isInitialized) {
        SDL_DestroyWindow(_window);
    }
}

void Spock::draw() { }

void Spock::run() {
    SDL_Event e;
    bool bQuit = false;

    while (!bQuit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) bQuit = true;
        }

        draw();
    }
}