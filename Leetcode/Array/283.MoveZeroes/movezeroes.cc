#include "solution.h"
#include <iostream>
#include <vector>
using namespace std;

int main()
{
    Solution s;
    vector<int> input{0,1,0,3,12};
    s.moveZeroes(input);
    for (const auto &i: input)
    {
        cout << i << endl;
    }
    return 0;
}

