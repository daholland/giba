//
// Created by daholland on 4/1/21.
//

#ifndef GIBA_VK_TEXTURES_H
#define GIBA_VK_TEXTURES_H

#include <vk_textures.h>
#include <spock.h>
#include <filesystem>
using namespace spock;

namespace vkutil {
    bool load_image_from_file(Spock& engine, std::filesystem::path file, AllocatedImage& outImage);
}



#endif //GIBA_VK_TEXTURES_H
