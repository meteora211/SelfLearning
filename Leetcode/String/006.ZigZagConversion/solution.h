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
	string convert_using_stl(string s, int numRows) {
        if(numRows <=1 ) return s;
        vector<string> storage(numRows);
        bool flag = true;
        int pos = 0;
        for(int i = 0; i < s.size(); i ++)
        {
            storage[pos] += s[i];
            if(pos == numRows - 1) flag = false;
            if(pos == 0) flag = true;
            flag ? pos++ : pos--;
        }
        
        string res = "";
        for(int j = 0; j < min(numRows,(int)s.size()); j ++)
        {
            res += storage[j];
        }
        return res;
    }
};
