class Solution {
public:
    vector<int> findDiagonalOrder(vector<vector<int>>& matrix) {
        vector<int> res;
        if(matrix.empty() || matrix[0].empty()) return res;
        int rowsize = matrix.size();
        int colsize = matrix[0].size();
        bool revers = true;
        for(int i = 0, j = 0; i < rowsize || j != colsize - 1; )
        {
            int curcol = j;
            int currow = i;
            while(i < rowsize && j >= 0)
            {
                res.push_back(matrix[i++][j--]);
            }
            if(curcol < colsize - 1 ) 
            {
                j = ++curcol;
                i = 0;
            }
            else
            {
                j = curcol;
                i = ++currow;
            }
            

        }
        return res;
    }
};
