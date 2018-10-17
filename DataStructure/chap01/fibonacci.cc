#include <iostream>
#include <limits>
using namespace std;

int fib(int n)
{
	return n < 3 ? 1 : fib(n-1)+fib(n-2);
}

int main()
{
	cout << "fib(1) = " << fib(1) << endl;
	cout << "fib(3) = " << fib(3) << endl;
	cout << "fib(5) = " << fib(5) << endl;
	cout << "fib(7) = " << fib(7) << endl;
}
