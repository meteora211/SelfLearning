#define CATCH_CONFIG_MAIN
#include "../Catch2/single_include/catch2/catch.hpp"
#include "solution.h"
#include <vector>
using std::vector;
TEST_CASE("stock2", "BuyAndSellStockII")
{
    Solution s;
    vector<int> case1{7,1,5,3,6,4};
    REQUIRE(s.maxProfit(case1)==7);
}
