#include "list.h"


int main()
{
    List<int> l;
    for(int i=5; i>0; i--)
    {
        l.insertAsFirst(i);
    }
    listprint(l);
}
