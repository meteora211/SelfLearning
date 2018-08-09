#define CATCH_CONFIG_MAIN
#include "../Catch2/single_include/catch2/catch.hpp"
#include "solution.h"
#include <iostream>
#include <vector>
using namespace std;
TEST_CASE("Test jump game","[JumpGame]")
{
    Solution s;
    vector<int> testcase1 {2,3,1,1,4};
    vector<int> testcase2 {3,2,1,0,4};
    vector<int> testcase3 {0,3,1,1,4};
    REQUIRE(s.canJump(testcase1)==true);
    REQUIRE(s.canJump(testcase2)==false);
    REQUIRE(s.canJump(testcase3)==false);
}
