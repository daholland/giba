find_package(Vulkan REQUIRED)
add_subdirectory("SDL")

add_subdirectory("sqlite_orm")

add_library(vkbootstrap STATIC)
add_library(glm INTERFACE)
add_library(vma INTERFACE)

add_library(stb_image INTERFACE)

add_library(tinyobjloader STATIC)

target_sources(vkbootstrap PRIVATE
        vk-bootstrap/src/VkBootstrap.h
        vk-bootstrap/src/VkBootstrap.cpp)

target_include_directories(vkbootstrap PUBLIC vk-bootstrap/src)
target_link_libraries(vkbootstrap PUBLIC Vulkan::Vulkan $<$<BOOL:UNIX>:${CMAKE_DL_LIBS}>)

target_include_directories(vma INTERFACE VulkanMemoryAllocator/src/)

target_include_directories(glm INTERFACE glm)

target_sources(tinyobjloader PRIVATE
        tinyobjloader/tiny_obj_loader.h
        tinyobjloader/tiny_obj_loader.cc
        )

target_include_directories(tinyobjloader PUBLIC tinyobjloader)

add_library(imgui STATIC)
target_include_directories(imgui PUBLIC ../extlib/imgui/ ../extlib/imgui/backends)
target_sources(imgui PRIVATE
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_demo.cpp
        imgui/backends/imgui_impl_sdl.cpp
        imgui/backends/imgui_impl_vulkan.cpp)

target_link_libraries(imgui PUBLIC Vulkan::Vulkan ${SDL2_LIBRARIES})

target_include_directories(stb_image INTERFACE stb)


#
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

