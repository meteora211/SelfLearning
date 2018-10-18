#include <cstdio>
#include <cstring>

class Solution
{
public:
    void replace(char str[], int length)
    {
        if(str == nullptr || length < 0)
            return;
        int effectiveLength = 0;
        int i = 0;
        int n = 0;                              //number of space
        while(str[i] != '\0')
        {
            ++ effectiveLength;
            if(str[i++] == ' ') n++;
        }
        int newLength = effectiveLength + 2 * n;
        int pos = effectiveLength;
        int newpos = newLength;
        while(newpos != pos && newpos > 0)
        {
            if(str[pos] != ' ')
            {
                str[newpos--] = str[pos--];
            }
            else
            {
                str[newpos--] = '0';
                str[newpos--] = '2';
                str[newpos--] = '%';
                pos --;
            }
        }
    }
};
