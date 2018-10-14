#define CATCH_CONFIG_MAIN
#include "AssignmentOperator.h"
#include "../Catch2/single_include/catch2/catch.hpp"
TEST_CASE("CMyString TESTCASE","CMyString")
{
    SECTION("Assign CMyString to another object")
    {
        char* text = "Hello World";
        CMyString str1 = text;
        CMyString str2;
        str2 = str1;
        str2.Print();
        REQUIRE(*(str2.getp()) == *text);
    }
    SECTION("Assign to itself")
    {
        char* text = "TestCase2";
        CMyString str1 = text;
        
        str1 = str1;
        str1.Print();
        REQUIRE(*(str1.getp()) == *text);
    }
    SECTION("Assign CMyString to another object")
    {
        char* text = "Hello World";
        CMyString str1 = text;
        CMyString str2,str3;
        str3 = str2 = str1;

        REQUIRE(*(str2.getp()) == *text);
        REQUIRE(*(str3.getp()) == *text);
    }
}
