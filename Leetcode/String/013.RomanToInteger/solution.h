class Solution {
public:
    int romanToInt(string s) {
        unordered_map<char,int> Roman = {{'I',1},{'V',5},{'X',10},{'L',50},{'C',100},{'D',500},{'M',1000}};
        int sum = Roman[s.back()];
        for(int i = s.size() - 2; i >= 0; i--)
        {
            if(Roman[s[i + 1]] > Roman[s[i]])
                sum = sum - Roman[s[i]];
            else
                sum = sum + Roman[s[i]];
        }
        return sum;
    }
};
