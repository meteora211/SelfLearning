#include <iostream>
#include <string>
using namespace std;

class Solution {
public:
    string convert(string s, int numRows) {
        string res("");
        string::size_type i = 0;
        bool flag = true;
        if (s.empty() || numRows <= 1 || numRows >= s.size())
            return s;
        for(int row = 0 ; row != numRows; row ++)
        {
            flag = true;
            for(string::size_type j = row; j < s.size(); )
            {
                res.push_back(s[j]);
                if(row == 0 || row == numRows - 1)
                {
                    j = j + numRows * 2 - 2;
                    continue;
                }
                if(flag)
                {
                    j = j + numRows * 2 - 2 - row * 2;
                    flag = false;
                }
                else
                {
                    j = j + 2 * row;
                    flag = true;
                }
                
            }
        }
        return res;
    }
};
