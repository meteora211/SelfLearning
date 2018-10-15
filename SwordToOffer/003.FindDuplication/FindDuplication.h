#include <cstdio>
#include <unordered_map>
using namespace std;
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
    bool duplicate_without_modify(int array [], int n, int* duplication);
};

//////////////////////////////////////////////////////////////
// array:       input array
// n:           array length
// duplication: duplicated number
// output:      
//    true:     find duplication in array
//    false:    no duplication
//////////////////////////////////////////////////////////////
bool Solution::duplicate(int array[], int n, int* duplication)
{
    if(array == nullptr || n == 0)
    {
        return false;
    }
    for(int j = 0; j < n; j++)
    {
        if(array[j] > n-1 || array[j] < 0)
        {
            return false;
        }
    }
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
//////////////////////////////////////////////////////////////
// array:       input array
// n:           array length
// duplication: duplicated number
// output:      
//    true:     find duplication in array
//    false:    no duplication
//////////////////////////////////////////////////////////////
bool Solution::duplicate_without_modify(int array [], int n, int* duplication)
{
    unordered_map<int,int> storage;
    for(int i = 0; i < n; i++)
    {
        if(array[i] > n-1 || array[i] < 0)
        {
            return false;
        }
        if(storage.find(array[i]) != storage.end())
        {
            *duplication = array[i];
            return true;
        }
        else
        {
            storage[array[i]] = i;
        }
    }
    return false;
}
