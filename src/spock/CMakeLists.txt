add_library(spock
        spock.cpp
        spock.h
        vk_types.h
        vk_init.h
        vk_init.cpp
        vk_mesh.cpp
        vk_mesh.h
        vk_textures.cpp
        vk_textures.h)

target_link_libraries(spock ${Vulkan_LIBRARIES})
target_include_directories(spock PRIVATE ./)

target_link_libraries(spock SDL2)

target_link_libraries(spock vkbootstrap vma glm tinyobjloader stb_image tracy)
target_link_libraries(spock imgui)
