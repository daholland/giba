cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_FLAGS "-v -fuse-ld=lld")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -v -fuse-ld=lld -lc++abi")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi -v -fuse-ld=lld")


project(giba)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory("src")