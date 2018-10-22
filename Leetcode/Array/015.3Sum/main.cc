#include "solution.h"
#include <iostream>
using namespace std;
int main()
{
    Solution s;
    vector<int> input = {-1, 0, 1, 2, -1, -4};
    vector<vector<int>> output = s.threeSum(input);
    cout << "[ " << endl;
    for(vector<vector<int>>::iterator iter = output.begin(); iter != output.end(); iter ++)
    {
        cout << "  [ ";
        for(vector<int>::iterator citer = iter->begin(); citer != iter -> end(); citer ++)
        {
            cout << *citer << " ";
        }
        cout << " ];" << endl;
    }
    cout << " ] " << endl;
}
