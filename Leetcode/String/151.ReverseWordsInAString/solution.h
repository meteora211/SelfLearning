class Solution {
public:
    void reverseWords(string &s) {
        if(s.empty()) return;
        int i = 0, j = 0;
        while(i < s.size() && s[i] == ' ') i++;
        if(i == s.size())
        {
            s = "";
            return;
        }
        s[0] = s[i++];
        while(i < s.size() )
        {
            if(s[i] == ' ' && s[j] == ' ' && i != j) i++;
            else s[++j] = s[i++];
        }
        if(s[j] != ' ') s[++j] = ' ';
        int pre = 0;
        for(int aft = 0; aft < j + 1; aft++)
        {
            if(s[aft] == ' ')
            {
                int temp = aft;
                while(aft > pre) swap(s[--aft], s[pre++]);
                aft = temp++;
                pre = temp;
            }
            
        }
        i = 0;
        int len = j;
        while(i < j) swap(s[i++],s[--j]);
        s = s.substr(0,len);
    }
};
