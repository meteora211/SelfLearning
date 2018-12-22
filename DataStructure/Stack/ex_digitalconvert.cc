#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

int convert(int num, int digit)
{
    int res = 0, count = 0;
    while(num > 0)
    {
        int rem = num % digit;
        num /= digit;
        res = rem * pow(10,count++) + res;
    }
    return res;
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("wrong input arguments\n");
        return 1;
    }
    int num = atoi(argv[1]), digit = atoi(argv[2]);
    int res = convert(num, digit);
    printf("Convert input number: %d to %d digit number------> %d", num, digit, res);
    return 0;
}
