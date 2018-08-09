class Solution {
public:
    vector<vector<int>> generate(int numRows) {
        vector<vector<int>> res;
        for (int i = 0; i != numRows; i ++)
        {
            vector<int> rows(i+1,1);
            res.push_back(rows);
        }
        
        if (numRows > 2)
        {
            for(int j = 2; j < numRows; j ++)
            {
                for (int k = 1; k < j; k ++)
                {
                    res[j][k] = res[j-1][k-1] + res[j-1][k];
                }
            }
        }
        
        return res;
    }
};
