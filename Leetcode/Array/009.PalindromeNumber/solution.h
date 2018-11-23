class Solution {
public:
    bool isPalindrome(int x) {
        if(x<0)
        {
            return false;
        }
        int p = 0;
        int xc = x;
        while(xc > 0)
        {
            p = p * 10 + xc % 10;
            xc = xc / 10;
        }
        return p == x;
		//return !(p ^ x);
    }
	bool isPalindrome_string(int x) {
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
