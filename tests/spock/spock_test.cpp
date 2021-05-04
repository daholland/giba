#include <catch2/catch_all.hpp>
namespace {
    int theAnswer() { return 6 * 9; } // function to be tested
}
TEST_CASE( "Spock_test", "[spock][foo]" ) {
    REQUIRE(theAnswer() == 54);
}