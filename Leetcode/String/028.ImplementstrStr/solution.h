#include <string>
using namespace std;
class Solution {
public:
    int strStr_naive(string haystack, string needle) {
        
        if(needle.empty())
            return 0;
        for(auto indexh = 0; indexh != haystack.size(); indexh ++)
        {
            
            for(string::size_type indexn = 0; indexn != needle.size(); indexn ++)
            {
                if( haystack[indexh +indexn] != needle[indexn])
                {
                    break;
                }
                else
                {
                    if (indexn == needle.size()-1)
                        return indexh;
                }
            }
            
                
        }
        return -1;
    }
};
