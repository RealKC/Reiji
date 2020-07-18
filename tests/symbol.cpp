#include <doctest/doctest.h>

#include <reiji/symbol.hpp>

TEST_SUITE("Symbol tests") {
    TEST_CASE("Correct values upon default construction") {
        reiji::symbol<int> symbol;

        REQUIRE_FALSE(symbol.is_valid());
        REQUIRE_FALSE(static_cast<bool>(symbol));
        REQUIRE(not symbol);
        REQUIRE(symbol == nullptr);
        REQUIRE(nullptr == symbol);
        REQUIRE_FALSE(symbol != nullptr);
        REQUIRE_FALSE(nullptr != symbol);
    }

    TEST_CASE("Failure upon trying to use a default constructed symbol") {
        reiji::symbol<int> s1;
        reiji::symbol<void()> s2;
        struct foo {
            int x;
        };
        reiji::symbol<foo> s3;
        REQUIRE_THROWS_AS(*s1, reiji::bad_symbol_access);
        REQUIRE_THROWS_AS(s2(), reiji::bad_symbol_access);
        REQUIRE_THROWS_AS(s3->x, reiji::bad_symbol_access);
    }

    TEST_CASE("Relational operators and shares_origin_with") {
        reiji::symbol<int> s1, s2;
        REQUIRE_FALSE(s1.shares_origin_with(s2));
        REQUIRE_FALSE(s1 == s2);
        REQUIRE(s2 != s1);
        REQUIRE_FALSE(s1 < s2);
        REQUIRE_FALSE(s1 > s2);
        REQUIRE_FALSE(s1 <= s2);
        REQUIRE_FALSE(s1 >= s2);
    }
}
