#pragma once
//A binSearch
template <typename T> static Rank binSearch(T* A, T const &e, Rank lo, Rank hi)
{
	while( lo < hi)
	{
		int mid = (lo + hi) >> 1;
		if(A[mid] < e) lo = mid + 1;
		else if(e < A[mid]) hi = mid;
		else return mid;
	}
	return -1;
}
