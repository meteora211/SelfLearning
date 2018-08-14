class Solution {
public:
    string longestCommonPrefix(vector<string>& strs) {
        if (strs.empty())
            return "";
        string res = strs[0];
        for (auto i=0; i !=strs.size(); i ++)
        {
            for(auto j = 0; j != strs[0].size(); j++)
            {
                if (strs[i][j] != res[j])
                {
                    res = res.substr(0, j);
                    break;
                }
            }
        }
        return res;
    }
};
