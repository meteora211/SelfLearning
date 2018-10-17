#include <iostream>
#include <algorithm>
using namespace std;

void reverse(int* , int, int);
void reverse(int* A, int n)
{
	reverse( A, 0, n-1);
}

void reverse(int* A, int lo, int hi)
{
	if(lo < hi)
	{
		swap(A[lo++], A[hi--]);
		reverse(A,lo,hi);
	}
}

int main()
{
	int A[10] = {1,2,3,4,5,6,7,8,9,10};
	reverse(A,10);
	for (int i = 0; i < 10; i ++)
	  cout<< A[i] << ' ';
	return 0;
}
