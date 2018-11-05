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

    List<int> l3;
    
    for(int i=9; i>2; i--)
    {
        l3.insertAsFirst(i);
        l3.insertAsFirst(i);
    }

    List<int> l4 = l3;

    printf("construct a duplicate list l3: ");
    listprint(l3);

    l3.deduplicate();
    printf("deduplicate l3: ");
    listprint(l3);

    printf("traverse l3: ");
    l3.traverse(Increase);
    listprint(l3);

    printf("traverse l3 again: ");
    l3.traverse(Increase);
    listprint(l3);
   
    int removed = l4.uniquify();
    printf("uniquify method for l4: %d", removed);
    listprint(l4);
}
