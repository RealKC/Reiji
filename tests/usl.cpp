#include <doctest/doctest.h>

// clang-format off
#include <reiji/unique_shared_lib.hpp>
#include <reiji/detail/push_platform_detection_macros.hpp>
// clang-format on

#if defined(__APPLE__)
#    define LIB1_NAME "liblib1.dylib"
#    define LIB2_NAME  "liblib2.dylib"
#elif REIJI_PLATFORM_POSIX
#    define LIB1_NAME "liblib1.so"
#    define LIB2_NAME  "liblib2.so"
#elif REIJI_PLATFORM_WINDOWS
#    define LIB1_NAME "lib1.dll"
#    define LIB2_NAME  "lib2.dll"
#endif

TEST_SUITE("unique_shared_lib behaviour") {
    TEST_CASE("unique_shared_lib behaves sanely after default construction") {
        reiji::unique_shared_lib lib;
        REQUIRE(lib.last_error().empty());
        REQUIRE(lib.get_symbol<int>("foobar") == nullptr);
        REQUIRE_FALSE(lib.get_symbol<char>("ninja") != nullptr);
        REQUIRE_FALSE(lib.last_error().empty());
        REQUIRE(lib.last_error()
                == "Cannot load symbol 'ninja' when no library was opened.");
    }

    TEST_CASE("unique_shared_lib is able to return symbols _that exist_ fine") {
        reiji::unique_shared_lib lib;
        lib.open(LIB1_NAME);
        auto bar = lib.get_symbol<int>("bar");
        REQUIRE_NOTHROW(*bar);
        REQUIRE(*bar == 5);
        auto ibar = lib.get_symbol<int()>("increase_bar_and_return_it");
        REQUIRE_NOTHROW(ibar());   // this call increased bar to 6
        REQUIRE(ibar() == 7);
    }

    TEST_CASE("symbols from different shared_libs are different") {
        reiji::unique_shared_lib lib1 {LIB1_NAME}, lib2 {LIB2_NAME};
        auto bar1 = lib1.get_symbol<int>("bar");
        auto bar2 = lib2.get_symbol<int>("bar");
        REQUIRE_FALSE(bar1.shares_origin_with(bar2));
        REQUIRE_FALSE(bar1 == bar2);
        REQUIRE(bar1 != bar2);
    }
}
