class Solution {
public:
    bool isPalindrome(int x) {
        if(x<0)
        {
            return false;
        }
        int x_copy = x;
        int revert = 0;
        while(x_copy)
        {
            revert = revert*10 + x_copy%10;
            x_copy = x_copy/10;
        }
        
        return !(revert ^ x);
    }
    bool isPalindrome_toString(int x) {
        if(x<0)
        {
            return false;
        }
        string s = std::to_string(x);
        bool res = true;
        int length = s.length();
        for(int i = 0; i != length/2; i ++)
        {
            res = res && (s[i] == s[length - 1 - i]);
        }
        return res;
    }
};
