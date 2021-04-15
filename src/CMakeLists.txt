#project(giba_prog)
#set(CMAKE_CXX_STANDARD 20)

#find_package(Vulkan REQUIRED)
#find_package(SDL2 REQUIRED)

add_subdirectory(gibalib)
add_subdirectory(spock)
add_subdirectory(shaders)









#
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
#
#execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")




