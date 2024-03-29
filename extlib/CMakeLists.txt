find_package(Vulkan REQUIRED)
add_subdirectory(SDL)

#add_subdirectory(Catch2)
add_subdirectory(sqlite_orm)

add_library(vkbootstrap STATIC)
add_library(glm INTERFACE)
add_library(vma INTERFACE)

add_library(stb_image INTERFACE)

add_library(tinyobjloader STATIC)

add_subdirectory(fmt)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
set(TRACY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tracy" CACHE FILEPATH "Path to Tracy")
add_library(tracy OBJECT "${TRACY_DIR}/TracyClient.cpp")
target_include_directories(tracy PUBLIC "${TRACY_DIR}")

target_link_libraries(tracy PRIVATE Threads::Threads)

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
        imgui/imgui.h
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_demo.cpp
        imgui/backends/imgui_impl_vulkan.cpp
        imgui/backends/imgui_impl_sdl.cpp)

target_link_libraries(imgui PUBLIC Vulkan::Vulkan SDL2)
message(sdl2_libraries: ${SDL2_LIBRARIES})

target_include_directories(stb_image INTERFACE stb)


#
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

