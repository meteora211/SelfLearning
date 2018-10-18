#define CATCH_CONFIG_MAIN
#include "../Catch2/single_include/catch2/catch.hpp"
#include "ReplaceSpaces.h"
#include <iostream>
#define LEN 100
TEST_CASE("ReplaceSpaces","TestCases")
{
    Solution s;
    
    SECTION("Given example in question stem")
    {
        char str[LEN] = "We are Happy.";
        s.replace(str,LEN);
        REQUIRE(strcmp(str,"We%20are%20Happy.") == 0);
    }
    SECTION("Test string without space inside")
    {
        char str[LEN] = "WeareHappy.";
        s.replace(str,LEN);
        REQUIRE(strcmp(str,"WeareHappy.") == 0);
    }
    SECTION("Test empty string")
    {
        char str[LEN] = "";
        s.replace(str,LEN);
        REQUIRE(strcmp(str,"") == 0);
    }
    SECTION("Test string only have space")
    {
        char str[LEN] = "   ";
        s.replace(str,LEN);
        REQUIRE(strcmp(str,"%20%20%20") == 0);
    }
    SECTION("space is at the begining of the string")
    {
        char str[LEN] = " test";
        s.replace(str,LEN);
        REQUIRE(strcmp(str,"%20test") == 0);
    }
    SECTION("spaces are at the end of the string")
    {
        char str[LEN] = "test   ";
        s.replace(str,LEN);
        REQUIRE(strcmp(str,"test%20%20%20") == 0);
    }
    SECTION("nullptr test")
    {
        char* str=nullptr;
        s.replace(str,LEN);
        REQUIRE(str == nullptr);
    }
}

