#include <catch2/catch_all.hpp>

namespace {
    int theAnswer() { return 6 * 9; } // function to be tested
}

TEST_CASE("Gibalib_test", "[gibalib][foo]") {
    using namespace Catch::literals;
    SECTION("float check") {
        float foo = 2.10000001;
        CAPTURE(CATCH_VERSION_MAJOR, CATCH_VERSION_MINOR, CATCH_VERSION_PATCH);
        REQUIRE(foo == 2.2_a);
    }

    SECTION("footest") {
        REQUIRE(theAnswer() == 42);
    }
}