cmake_minimum_required(VERSION 3.17)
project(giba)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)

add_subdirectory("src")
add_subdirectory("extlib")

target_link_options(giba PUBLIC "LINKER:-v")