#include <iostream>
#include <algorithm>
using namespace std;

void bubblesort(int arr[], int n)
{
	bool sorted = false;
	while(n)
	{
		sorted = true;
		for(int i = 0; i < n - 1; i++)
		{
			if(arr[i] < arr[i + 1])
			{
				swap(arr[i],arr[i+1]);
				sorted = false;
			}
		}
		n --;
		if(sorted)
		{
			break;
		}
	}
}

int main()
{
	int arr[10] = {3,2,4,0,1,7,6,8,9,5};
	bubblesort(arr, 10);
	for(int i = 0; i < 10; i ++)
	  cout << arr[i] << ' ';
	return 0;
}
