project(giba_prog)
set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)


include_directories(${SDL2_INCLUDE_DIRS})
add_executable(giba main.cpp gibalib/gibalib.cpp gibalib/gibalib.h)
target_link_libraries(giba ${VULKAN_LIB_LIST})
target_link_libraries(giba ${SDL2_LIBRARIES})