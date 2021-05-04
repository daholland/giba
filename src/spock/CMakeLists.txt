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

target_include_directories(spock PRIVATE ./)
#
#set_target_properties(spock PROPERTIES COMPILE_FLAGS "-fsanitize=memory -fno-omit-frame-pointer -fPIE -fPIC -fsanitize-memory-track-origins -fno-optimize-sibling-calls -g -O1")
#set_target_properties(spock PROPERTIES LINK_FLAGS "-fsanitize=memory -fno-omit-frame-pointer -fPIE -fPIC -fsanitize-memory-track-origins -fno-optimize-sibling-calls -g -O1")


target_link_libraries(spock ${Vulkan_LIBRARIES})

target_link_libraries(spock SDL2)

target_link_libraries(spock vkbootstrap vma glm tinyobjloader stb_image tracy)
target_link_libraries(spock imgui)
