#include "solution.h"
#include <iostream>
using namespace std;
int main()
{
    Solution s;
    vector<int> input = {-1, 2, 1, -4};
    int output = s.threeSumClosest(input,1);
    cout << "result is: " << output  << endl;
} 
