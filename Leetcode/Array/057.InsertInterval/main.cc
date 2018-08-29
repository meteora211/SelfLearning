#include "solution.h"
#include <iostream>
using namespace std;
int main()
{
	vector<Interval> intervals;
	vector<Interval> output;
	intervals.push_back(Interval(1,3));
	intervals.push_back(Interval(6,9));
	Interval newInterval{2,5};
	Solution s;
	output = s.insert(intervals,newInterval);
	for(auto i : output)
	{
		cout << '[' << i.start << ',' << i.end;
		cout << ']' << endl;
	}
	return 0;
}
