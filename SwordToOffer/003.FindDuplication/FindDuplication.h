#include <cstdio>
class Solution
{
public:
    void swap(int* a, int* b)
    {
        int temp = *a;
        *a = *b;
        *b = temp;
    }
    bool duplicate(int array[], int n, int* duplication);
};

bool Solution::duplicate(int array[], int n, int* duplication)
{
    printf("adress of input array in function call: %d\n", array);
    for(int i = 0; i < n; i++)
    {
        while(array[i] != i)
        {
            if(array[i] == array[array[i]]) 
            {
                //Error 1
                //duplication = array + i;
                *duplication = array[i];
                return true;
            }
            swap(array + i, array + array[i]);
        }
    }
    return false;
}
