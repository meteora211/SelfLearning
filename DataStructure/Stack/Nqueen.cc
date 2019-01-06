#include "Stack.h"
#include <cstdio>
using namespace std;
class NQueen{
public:
    int x, y;
    NQueen(int xx = 0, int yy = 0): x(xx), y(yy) {};
    bool operator == (NQueen const & q)
    {
        return  (x == q.x) 
                || (y == q.y)
                || (y + x == q.x + q.y)
                || (y + x == q.x + q.y);
    }
    bool operator != (NQueen const & q){return ! (*this == q);}
    void Print()
    {
        printf("%*s*\n",y, " ");
    }
private:
    int _size = 4;
};


void placeNQueen(int n, Stack<NQueen>& solution)
{
    NQueen q(0,0);
    while(solution.size() < n)
    {
        if(n <= q.y )
        {
            q = solution.pop(); q.y++;
        }
        else
        {
            while(q.y < n && 0 <= solution.find(q))
            {
                q.y++;
            }
            if(q.y < n)
            {
                solution.push(q);
                q.y = 0; q.x++;
            }
        }
    }
}

int main()
{
    int size = 4;
    Stack<NQueen> solution;
    placeNQueen(size,solution);
    for(int i = 0; i < solution.size(); i++)
    {
        solution[i].Print();
    }

    return 0;
}
