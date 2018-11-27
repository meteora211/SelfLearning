class Solution {
public:
    string reverseWords(string s) {
        if(s.empty()) return s;
        int i = 0, j = 0;
        while(i++ < s.size())
        {
            if(i == s.size() || s[i] == ' ')
            {
                int temp = i;
                while(i > j) swap(s[--i],s[j++]);
                i = temp; j = ++temp;
            }
        }
        return s;
    }
};
