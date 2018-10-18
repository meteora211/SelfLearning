#include "ReplaceSpaces.h"
#include <iostream>
int main()
{
    const int length = 20;
    //char str[13] = {"We are Happy."};
    char str[length] = "We are Happy.";
    Solution s;
    s.replace(str,length);
    
/*    for(int i = 0; str[i]; i ++)
    {
        //printf('%s',str[i]);
        //std::cout<<str[i];
//        printf("%c",str[i]);
    }
*/
    return 0;
}
