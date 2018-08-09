#include "solution.h"
#include <iostream>
using namespace std;

int main()
{
    Solution s;
    vector<int> input {9,9,9};
    vector<int> output;
    output = s.plusOne(input);
    for (const auto &element : output)
        cout << element << endl;
    return 0;
}
