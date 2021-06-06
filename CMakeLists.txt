cmake_minimum_required(VERSION 3.16)
project(giba)

set(CMAKE_CXX_STANDARD 17)#temporarily 17 due to clion 2020.1 "std::ranges" bug:https://youtrack.jetbrains.com/issue/CPP-20238

#set(ENV{ASAN_OPTIONS} "symbolize=1:suppressions=${CMAKE_SOURCE_DIR}/san.suppress")
#set(ENV{LSAN_OPTIONS} "suppressions=${CMAKE_SOURCE_DIR}/san.suppress")
#set(ENV{ASAN_SYMBOLIZER_PATH} "/usr/bin/llvm-symbolizer")

#set(GIBA_SANITIZE "-fsanitize=address -fsanitize-blacklist=${CMAKE_SOURCE_DIR}/san.suppress -fno-omit-frame-pointer -fsanitize-recover=address -g -O1")
#set(GIBA_SANITIZE "-fsanitize=leak ")
#set(GIBA_SANITIZE "-fsanitize=thread")
set(GIBA_SANITIZE "-fsanitize=undefined -fno-omit-frame-pointer")

#NOTE! ALSO LOOK AT BOTTOM FOR LIB OPTIONS
#set(GIBA_SANITIZE "-fsanitize=memory -fno-omit-frame-pointer -fPIE -fPIC -fsanitize-memory-track-origins -fno-optimize-sibling-calls -g -O1")

message(GIBAMSG ${GIBA_SANITIZE})

find_package(Vulkan REQUIRED)
#find_package(SDL2 REQUIRED)



add_subdirectory("src")
add_subdirectory("extlib")

target_compile_definitions(tracy PUBLIC TRACY_ENABLE)

add_executable(giba src/main.cpp src/ui.cpp src/ui.h src/config.cpp src/config.h)
set_target_properties(giba PROPERTIES COMPILE_FLAGS "${GIBA_SANITIZE}")
set_target_properties(giba PROPERTIES LINK_FLAGS "${GIBA_SANITIZE}")

target_link_libraries(giba gibalib spock fmt::fmt)
target_include_directories(giba
        PRIVATE
        src/gibalib
        src/spock
        extlib/imgui_extras)


add_subdirectory(tests)

add_dependencies(giba Shaders)

set(CMAKE_C_FLAGS "-fuse-ld=lld")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GIBA_SANITIZE}")

set(CMAKE_CXX_FLAGS "-fuse-ld=lld")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GIBA_SANITIZE}")

message(GIBACMAKEFLAGS ${CMAKE_CXX_FLAGS})
message(GIBACMAKEFLAGS ${CMAKE_C_FLAGS})
message(GIBASAN_CMAKEFLAGS ${GIBA_SANITIZE})
message(ASANOPTIONS $ENV{ASAN_OPTIONS})
message(ASANSERIALIZERPATH $ENV{ASAN_SYMBOLIZER_PATH})

#set_target_properties(gibalib PROPERTIES COMPILE_FLAGS "-fsanitize=memory -fno-omit-frame-pointer -fPIE -fPIC  -fsanitize-memory-track-origins -fno-optimize-sibling-calls -g -O1")
#set_target_properties(gibalib PROPERTIES LINK_FLAGS "-fsanitize=memory -fno-omit-frame-pointer -fPIE -fPIC  -fsanitize-memory-track-origins -fno-optimize-sibling-calls -g -O1r")
#set_target_properties(spock PROPERTIES COMPILE_FLAGS "-fsanitize=memory -fno-omit-frame-pointer -fPIE -fPIC  -fsanitize-memory-track-origins -fno-optimize-sibling-calls -g -O1")
#set_target_properties(spock PROPERTIES LINK_FLAGS "-fsanitize=memory -fno-omit-frame-pointer -fPIE -fsanitize-memory-track-origins -fno-optimize-sibling-calls -g -O1")
