#define CATCH_CONFIG_MAIN
#include "../Catch2/single_include/catch2/catch.hpp"
#include "solution.h"
#include <iostream>
#include <vector>
using std::vector;

TEST_CASE("Remove Element", "removeElement")
{
    SECTION("1")
    {
        int arri[] = {1,2,3,4,5,6,7,8,9};
        vector<int> arr(arri,arri+9);
        Solution s;
        REQUIRE(s.removeElement(arr, 8) == 8);
    }

}

