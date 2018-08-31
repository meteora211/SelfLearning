#include "solution.h"
#include <iostream>
int main()
{
	Solution s;
	vector<Interval> input;
	input.push_back(Interval(1,3));
	input.push_back(Interval(2,6));
	input.push_back(Interval(8,10));
	input.push_back(Interval(15,18));

	vector<Interval> output;
	output = s.merge(input);
	for(auto a : output)
	{
		cout << '[' << a.start << ',';
		cout << a.end << ']' << endl;
	}
}
