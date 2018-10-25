class Solution {
public:
    string countAndSay(int n) {
        if(n <= 0)
            return "1";
        string res = "1";
        for(int i = 0; i < n - 1; i++)
        {
            string temp;
            int count = 1;

            for(int j = 0; j < res.size(); j++)
            {
                if(res[j + 1] == res[j] && j < res.size() - 1 )
                {
                    count ++;
                }
                else
                {
                    temp += std::to_string(count) + res[j];
                    count = 1;
                }
            }
            
            
            res = temp;
        }
        return res;
    }
};
