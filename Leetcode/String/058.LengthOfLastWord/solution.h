#include <string>
#include <cctype>
using namespace std;
class Solution {
public:
    int lengthOfLastWord(string s) {
        int len = 0;
        for(string::size_type i = 0; i != s.size();)
        {
            if(s[i++] == ' ')
            {
                if(s[i] != ' ' && i != s.size())
                {
                    len = 0;
                }
            }
            else
            {
                len ++;
            }
        }
        return len;
    }
};
