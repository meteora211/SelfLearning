#include <cstdio>
#include <cstring>

class Solution
{
public:
    void replace(char str[], int length)
    {
        int n = 0;
        for (int i = 0; i < length; i ++)
        {
            if(str[i] == ' ') n++;
        }
        int nlen = length + 2 * n;
        int pos = length;
        while(nlen)
        {
            printf("nlen is %d, pos is %d\n", nlen, pos);
            if(str[pos] != ' ')
            {
                str[nlen--] = str[pos--];
            }
            else
            {
                str[nlen--] = '0';
                str[nlen--] = '2';
                str[nlen--] = '%';
                pos --;
            }
        }
    }
};
