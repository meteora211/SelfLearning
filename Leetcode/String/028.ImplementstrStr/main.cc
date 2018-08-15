#include "solution.h"
#include <iostream>
using namespace std;

int main()
{
    Solution s;
    string haystack("mississippi");
    string needle = "issip";
    
//    string haystack("hello");
//    string needle = "ll";
    int res = s.strStr_KMP(haystack, needle);
    cout << "result is " << res << endl;

    
    return 0;
}
