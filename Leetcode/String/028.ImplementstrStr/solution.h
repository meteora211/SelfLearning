#include <vector>
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
int strStr_KMP(string haystack, string needle) {
        
        if(needle.empty())
            return 0;
        vector<int> pi = prefix(needle);
        int q = 0;
        for(auto indexh = 0; indexh != haystack.size(); indexh ++)
        {
            
            while(q>0 && needle[q] != haystack[indexh])
                q = pi[q - 1];
            if (needle[q] == haystack[indexh])
                q++;
            if (q == needle.size())
                return indexh - q + 1;
                
        }
        return -1;
    }
    
private:
    vector<int> prefix(string s)
    {
        int len = s.length();
        vector<int> pi(len,0);
        int k = 0;
        pi[0] = 0;
        for (decltype(s.size()) q = 1; q<s.size();q++)
        {
            while(k > 0 && s[k] != s[q])
                k = pi[k-1];
            if (s[k] == s[q])
            {
                k++;
            }
            pi[q] = k;
        }
        return pi;
    }
};
