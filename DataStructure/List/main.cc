#include "list.h"
#include <cstdio>
#include <stdlib.h>
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

    Posi(int) p = l4.search(5,7,l4.last());
    printf("search 5 in l4: ");
    listnodeprint(p);

    List<int> l5;
    for(int i = 0; i < 15; i++)
    {
        l5.insertAsFirst(rand()%20);
    }

    List<int> l6 = l5;
    printf(" new random list l5: ");
    listprint(l5);

    printf("insertionSort l5: ");
    l5.insertionSort(l5.first(),l5.size());
    listprint(l5);

    printf("random list l6: ");
    listprint(l6);
    l6.selectionSort(l6.first(),l6.size());
    listprint(l6);
}
