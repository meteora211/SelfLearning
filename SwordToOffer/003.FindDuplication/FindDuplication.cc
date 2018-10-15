#define CATCH_CONFIG_MAIN
#include "FindDuplication.h"
#include "../Catch2/single_include/catch2/catch.hpp"

TEST_CASE("FindDuplication","Problem1")
{
    Solution s;
    SECTION("duplicated number is minmal")
    {
        int numbers[] = {2,1,3,1,4};
        printf("adress of numbers: %d\n",numbers);
        int expected = 1;
        // Segment fault 2
        //int* duplication = nullptr;
        int duplication[] = {0};
        REQUIRE(true == s.duplicate(numbers, 5, duplication));
        REQUIRE(*duplication ==  expected);
    }
}
