project(giba_prog)
set(CMAKE_CXX_STANDARD 20)

#find_package(Vulkan REQUIRED)
#find_package(SDL2 REQUIRED)




add_executable(giba
            main.cpp
            gibalib/gibalib.cpp
            gibalib/gibalib.h
            spock/spock.cpp
            spock/spock.h
            spock/vk_types.h
            spock/vk_init.h
            spock/vk_init.cpp)

target_include_directories(giba PUBLIC spock/ gibalib/)
target_link_libraries(giba ${Vulkan_LIBRARIES})

target_link_libraries(giba ${SDL2_LIBRARIES})

target_link_libraries(giba imgui)

#
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
#
#execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")




