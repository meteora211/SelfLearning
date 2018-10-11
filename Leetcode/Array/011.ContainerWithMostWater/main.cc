#include "solution.h"
#include <iostream>
using namespace std;
int main()
{
    vector<int> input{1,8,6,2,5,4,8,3,7};
    Solution s;
    int out = s.maxArea(input);
    cout << "output is " << out << endl;
    return 0;
}
