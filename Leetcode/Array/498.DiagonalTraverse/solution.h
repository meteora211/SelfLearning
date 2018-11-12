class Solution {
public:
    vector<int> findDiagonalOrder(vector<vector<int>>& matrix) {
        vector<int> res;
        if(matrix.empty() || matrix[0].empty()) return res;
        int rowsize = matrix.size();
        int colsize = matrix[0].size();
        bool reverse = true;
        int index = 0;
        for(int i = 0, j = 0; index < colsize + rowsize - 1; )
        {
            
            
            if(!reverse)
            {
                while(i < rowsize && j >= 0)
                {
                    res.push_back(matrix[i++][j--]);
                }
                reverse = !reverse;
            }
            else
            {
                while(i >= 0  && j < colsize)
                {
                    res.push_back(matrix[i--][j++]);
                }
                reverse = !reverse;
            }
            
            index++;

            if(index <= max(colsize,rowsize) - 1 ) 
            {
                reverse? j = max(0, index - rowsize + 1) : j = index;
                reverse? i = index : i = max(0, index - colsize + 1);
            }
            else
            {
                reverse? j = max(index - colsize + 1, 2*index - rowsize - colsize + 1) : j = colsize - 1;
                reverse? i = rowsize - 1 : i = max(index - rowsize + 1, 2*index - rowsize - colsize + 1);
            }
            i = min(i, rowsize - 1);
            j = min(j, colsize - 1);

        }
        return res;
    }
};
