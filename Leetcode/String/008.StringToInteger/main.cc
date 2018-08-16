#include <iostream>
#include "solution.h"
using namespace std;

int main()
{
	Solution s;
	string input("4193 with words");
	int output = s.myAtoi(input);
	cout << output << endl;
	return 0;
}
