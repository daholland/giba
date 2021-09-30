#find_package(Catch2 3) #no idea why 3 got installed with cmake when a v2 branch was checked out.... TODO
add_executable(giba_tests
        giba_tests.cpp
        gibalib/gibalib_test.cpp
        spock/spock_test.cpp gibalib/opcodes.cpp)

target_link_libraries(giba_tests PRIVATE Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(giba_tests)

