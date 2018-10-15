#define CATCH_CONFIG_MAIN
#include "FindDuplication.h"
#include "../Catch2/single_include/catch2/catch.hpp"

TEST_CASE("FindDuplication","Problem1")
{
    Solution s;
    SECTION("duplicated number is minmal")
    {
        int numbers[] = {2,1,3,1,4};
        // printf("adress of numbers: %d\n",numbers);
        int expected = 1;
        // Segment fault 2
        // int* duplication = nullptr;
        int duplication[] = {0};
        REQUIRE(true == s.duplicate(numbers, 5, duplication));
        REQUIRE(*duplication ==  expected);
    }

    SECTION("duplicated number is maximum")
    {
        int numbers[] = {2,4,3,1,4};
        int expected = 4;
        int duplication[] = {0};
        REQUIRE(true == s.duplicate(numbers, 5, duplication));
        REQUIRE(*duplication ==  expected);
    }

    SECTION("invalid input array with negative value")
    {
        int numbers[] = {-1,4,2,1,3};
        int duplication[] = {0};
        REQUIRE(false == s.duplicate(numbers, 5, duplication));
    }

    SECTION("invalid input array with value out of range")
    {
        int numbers[] = {10,4,2,1,3};
        int duplication[] = {0};
        REQUIRE(false == s.duplicate(numbers, 5, duplication));
    }

    SECTION("empty input array")
    {
        int numbers[] = {-1,4,2,1,3};
        int duplication[] = {0};
        REQUIRE(false == s.duplicate(numbers, 0, duplication));
    }

    SECTION("nullptr array")
    {
        int duplication[] = {0};
        REQUIRE(false == s.duplicate(nullptr, 5, duplication));
    }

    SECTION("array without duplication")
    {
        int numbers[] = {0,4,2,1,3};
        int duplication[] = {0};
        REQUIRE(false == s.duplicate(numbers, 5, duplication));
    }

}

TEST_CASE("FindDuplication_without_modify","Problem2")
{
    Solution s;
    SECTION("duplicated number is minmal")
    {
        int numbers[] = {2,1,3,1,4};
        int* num_copy = numbers;
        // printf("adress of numbers: %d\n",numbers);
        int expected = 1;
        int duplication[] = {0};
        REQUIRE(true == s.duplicate_without_modify(numbers, 5, duplication));
        REQUIRE(*duplication ==  expected);
        REQUIRE(numbers == num_copy);
    }
}
