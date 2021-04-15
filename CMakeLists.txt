cmake_minimum_required(VERSION 3.17)
project(giba)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)
#find_package(SDL2 REQUIRED)



add_subdirectory("src")
add_subdirectory("extlib")

target_compile_definitions(tracy PUBLIC TRACY_ENABLE)

add_executable(giba src/main.cpp)
target_link_libraries(giba gibalib spock)
target_include_directories(giba PRIVATE src/gibalib src/spock)


add_dependencies(giba Shaders)

set(CMAKE_C_FLAGS "-fuse-ld=lld")


set(CMAKE_CXX_FLAGS "-fuse-ld=lld")