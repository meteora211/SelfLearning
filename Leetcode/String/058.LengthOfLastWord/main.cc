#include <iostream>
#include "solution.h"
using namespace std;

int main()
{
	Solution s;
	string input("aa  ");
	int res = s.lengthOfLastWord(input);
	cout << "res is: " << res << endl;
	return 0;
}
