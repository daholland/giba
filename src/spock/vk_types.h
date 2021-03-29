//
// Created by daholland on 3/23/21.
//

#ifndef GIBA_VK_TYPES_H
#define GIBA_VK_TYPES_H

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace spock {
 struct AllocatedBuffer {
     VkBuffer _buffer;
     VmaAllocation _allocation;
 };
 struct AllocatedImage {
     VkImage _image;
     VmaAllocation _allocation;
 };
}

#endif //GIBA_VK_TYPES_H
