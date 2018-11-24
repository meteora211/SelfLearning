class Solution {
public:
    string longestPalindrome(string s) {
        if(s.size() <= 1) return s;
        int i = 0;
        string res;
        res.push_back(s[0]);
        bool ispalindrome = false;
        while(i < s.size())
        {
            int j = s.size() - 1;
            int tempi = i;
            int tempj = j;
            while(j > tempi)
            {
                if(s[tempi] == s[j])
                {
                    ispalindrome = true;
                    tempi++; j--;
                }
                else
                {
                    ispalindrome = false;
                    tempi = i; 
                    j = --tempj;
                }
            }
            if(ispalindrome && tempj - i >= res.size()) res = s.substr(i, tempj - i + 1);
            i++;
        }
        return res;
    }
};
