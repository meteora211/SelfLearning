#include "solution.h"
#include <iostream>
using namespace std;

int main()
{
    Solution s;
    string haystack("mississippi");
    string needle = "issip";
    int res = s.strStr_naive(haystack, needle);
    cout << "result is " << res << endl;
    return 0;
}
