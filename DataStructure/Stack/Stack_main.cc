#include "Stack.h"
#include <time.h>
#include <math.h>
#include <stdio.h>
using namespace std;

template <typename T>
void StackInitializer(Stack<T>& S , T n)
{
    int i = 0;
    while(i++ < n)
    {
        T e = (T) rand() % (2 * n);
        S.push(e);
    }
}

int main()
{
    srand ( ( unsigned int ) time ( NULL ) ); //设置随机种子
    Stack<int> s;
    StackInitializer(s, 5);
    printf("initialize Stack: \n");
    s.Print();
    Stack<int> a, b;
    
    while(!s.empty() || !a.empty())
    {
        if((rand()%2 || a.empty()) && !s.empty())
            a.push(s.pop());
        else
            b.push(a.pop());
    }

    printf("Stack permutation: \n");
    b.Print();
    return 0;
}
