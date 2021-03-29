#project(giba_prog)
#set(CMAKE_CXX_STANDARD 20)

#find_package(Vulkan REQUIRED)
#find_package(SDL2 REQUIRED)

find_program(GLSL_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)


add_executable(giba
            main.cpp
            gibalib/gibalib.cpp
            gibalib/gibalib.h
            spock/spock.cpp
            spock/spock.h
            spock/vk_types.h
            spock/vk_init.h
            spock/vk_init.cpp spock/vk_mesh.cpp spock/vk_mesh.h)

target_include_directories(giba PUBLIC spock/ gibalib/)
target_link_libraries(giba ${Vulkan_LIBRARIES})

target_link_libraries(giba ${SDL2_LIBRARIES})

target_link_libraries(giba vkbootstrap vma glm tinyobjloader)
target_link_libraries(giba imgui)

target_link_libraries(giba sqlite_orm::sqlite_orm)


file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.comp"
        )

foreach(GLSL ${GLSL_SOURCE_FILES})
    message(STATUS "BUILDING SHADER")
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${CMAKE_CURRENT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
    message(STATUS ${GLSL})
    message(glslval ${GLSL_VALIDATOR})
    message(spirv ${SPIRV})
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

message(STATUS "${SPIRV_BINARY_FILES}")

add_custom_target(
        Shaders ALL
        COMMENT "building shaders..."
        DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(giba Shaders)

#
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
#
#execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")




