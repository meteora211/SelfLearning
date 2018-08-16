#include <string>
#include <limits.h>
using namespace std;
class Solution {
public:
    int myAtoi(string str) {
        long res = 0;
        int sign = 1;
        int i = str.find_first_not_of(' ');
        if(str[i] == '+' || str[i] == '-')
        {
            str[i] == '-' ? sign = -1: sign = 1;
            i ++;
        }
        for( i ; i != str.size(); i++)
        {
            if(str[i] > '9' || str[i] < '0')
            {
                break;
            }
            else
            {
                res = res * 10 + (str[i] - '0');
                if(res * sign > INT_MAX) return INT_MAX;
                if(res * sign < INT_MIN) return INT_MIN;
            }
            
        }
        return res * sign;
    }
};
