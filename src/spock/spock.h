//
// Created by daholland on 3/23/21.
//
// kudos to vkguide / https://github.com/vblanco20-1/vulkan-guide

#ifndef GIBA_SPOCK_H
#define GIBA_SPOCK_H
#include <vk_types.h>

class Spock {
public:

    bool _isInitialized{false};
    int _frameNumber {0};

    VkExtent2D _windowExtent{1700, 900};

    struct SDL_Window* _window{nullptr};

    void init();

    void cleanup();

    void draw();

    void run();
};


#endif //GIBA_SPOCK_H
