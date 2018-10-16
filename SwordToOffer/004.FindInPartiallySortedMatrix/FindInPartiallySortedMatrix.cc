#define CATCH_CONFIG_MAIN
#include "FindInPartiallySortedMatrix.h"
#include "../Catch2/single_include/catch2/catch.hpp"

TEST_CASE("FindInPartiallySortedMatrix","TestCases")
{
    Solution s;
    SECTION("target number exists in matrix")
    {
        int matrix[] = {1,2,8,9,2,4,9,12,4,7,10,13,6,8,11,15};
        int number = 7;
        int column = 4;
        int row = 4;
        REQUIRE(true == s.Find(matrix,column,row,number));
    }
    SECTION("target number doesn't exist in matrix")
    {
        int matrix[] = {1,2,8,9,2,4,9,12,4,7,10,13,6,8,11,15};
        int number = 5;
        int column = 4;
        int row = 4;
        REQUIRE(false == s.Find(matrix,column,row,number));
    }
    SECTION("invalid input matrix")
    {
        int* matrix = nullptr;
        int number = 5;
        int column = 4;
        int row = 4;
        REQUIRE(false == s.Find(matrix,column,row,number));
    }

    SECTION("target number is minimal value in matrix")
    {
        int matrix[][4] = {1,2,8,9,2,4,9,12,4,7,10,13,6,8,11,15};
        int number = 1;
        int column = 4;
        int row = 4;
        REQUIRE(true == s.Find((int*)matrix,column,row,number));
    }
    SECTION("target number is maximum value in matrix")
    {
        int matrix[] = {1,2,8,9,2,4,9,12,4,7,10,13,6,8,11,15};
        int number = 15;
        int column = 4;
        int row = 4;
        REQUIRE(true == s.Find(matrix,column,row,number));
    }
}
