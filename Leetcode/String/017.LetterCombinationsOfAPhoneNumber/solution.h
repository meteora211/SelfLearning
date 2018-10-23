class Solution {
public:
    vector<string> letterCombinations(string digits) {
        vector<string> kb = {"","","abc","def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"};
        vector<string> res;
        if (digits.empty())
            return res;
        res.push_back("");
        for(int i = 0; i < digits.size(); i ++)
        {
            vector<string> temp;
            int index = digits[i] - '0';
            string inputs = kb[index];
            for(vector<string>::iterator iter = res.begin(); iter != res.end(); iter++)
            {
                for(auto& c:inputs)
                {
                    temp.push_back(*iter + c);
                }
            }
            res = temp;
        }
        return res;
    }
};
