#include <cstdio>

class Solution{
public:
    bool Find(int* matrix, int column, int row, int number)
    {
        int i = 0;
        int j = column - 1;
        if(matrix == nullptr || column < 0 || row < 0)
        {
            return false;
        }
        while(i < row && j >= 0)
        {
            if(number == *(matrix + i * column + j))
            {
                return true;
            }
            else
            {
                number > *(matrix + i * column + j) ? i++ : j--;
            }
        }
        return false;
    }
};
