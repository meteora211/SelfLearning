#include "list.h"
#include <cstdio>
using namespace std;

int main()
{
    List<int> l;
    for(int i=5; i>0; i--)
    {
        l.insertAsFirst(i);
    }
    listprint(l);
    printf("Testing [] operator: 3 is expected.\n");
    printf("%d\n",l[2]);

    List<int> l2(l);
    l.remove(l.first());
    
    printf("Testing initializer: l2 should be same with l.\n");
    listprint(l2);
    
    printf("Testing remove(): first listnode in l should be removed.\n");
    listprint(l);
    
}
