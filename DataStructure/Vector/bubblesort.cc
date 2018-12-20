#include <cstdio>
#include <algorithm>
using namespace std;

template <typename T> 
void bubblesort_origin(T A[], int n)
{
    for(int i = 0; i < n - 1; i++)
    {
        for(int j = 0; j < n - i - 1; j++)
        {
            if(A[j] > A[j + 1])
            {
                swap(A[j], A[j + 1]);
            }
        }
    }
}

template <typename T>
void bubblesort_faster(T A[], int n)
{
    bool sorted = false;
    while(!sorted)
    {
        sorted = true;
        for(int i = 0; i < n - 1; i++)
        {
            if(A[i] > A[i + 1])
            {
                swap(A[i], A[i + 1]);
                sorted = false;
            }
        }
        n--;
    }
}

template <typename T>

template <typename T>
void print_array(T A[], int n)
{
    int i = 0;
    while( i < n )
    {
        printf("%d ",A[i]);
        i++;
    }
    printf("\n");
}

int main()
{
    int A[] = {3,6,2,4,6,9,0,3,1};
    int B[] = {3,6,2,4,6,9,0,3,1};
    int len = 9;
    
    print_array(A,len);
    
    bubblesort_origin(A,len);
    bubblesort_faster(B,len);

    print_array(A,len);
    print_array(B,len);

    return 0;
}   
