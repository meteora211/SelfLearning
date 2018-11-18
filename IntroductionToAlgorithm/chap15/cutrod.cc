#include <algorithm>
#include <vector>
#include <iostream>
#include <unordered_map>
using namespace std;

class CutRod{
private:
    vector<int> p;                  //初始化的价格表
    unordered_map<int,int> r;       //用于记录子问题解的备忘录
public:
    CutRod() {p = {1,5,8,9,10,17,17,20,24,30};};
    CutRod(vector<int> price) : p(price) {};
    int RecurseCutRod(int n);
    int MemorizedCutRod(int n);
    int ButtomUpCutRod(int n);

};

int CutRod::RecurseCutRod(int n)
{
    int q = 0;
    for(int i = 0; i < n; i++)
    {
        q = max(q, RecurseCutRod(n-i-1) + p[i]);
    }
    return q;
}

int CutRod::MemorizedCutRod(int n)
{
    int q = 0;
    if(r.find(n) != r.end()) return r[n];
    else
    {
        for(int i = 0; i < n; i++)
        {
            q = max(q, MemorizedCutRod(n-i-1) + p[i]);
        }
    }
    r[n] = q;
    return q;
}

int CutRod::ButtomUpCutRod(int n)
{
    int q = 0;
    unordered_map<int,int> br;
    br[0] = 0;
    for(int i = 1; i <= n; i++)     //i代表规模为i的子问题
    {
        for(int j = 0; j < i; j++)  //对规模为i的子问题进行求解
        {
            q = max(q, p[j] + br[i - j - 1]);
        }
        br[i] = q;
    }
    return br[n];
}

int main()
{
    CutRod c;
    cout << "Rec: max p with lenth 1:  " << c.RecurseCutRod(1) << endl;
    cout << "Rec: max p with lenth 3:  " << c.RecurseCutRod(3) << endl;
    cout << "Rec: max p with lenth 6:  " << c.RecurseCutRod(6) << endl;
    cout << "Rec: max p with lenth 10:  " << c.RecurseCutRod(10) << endl;
    
    cout << "Mem: max p with lenth 1:  " << c.MemorizedCutRod(1) << endl;
    cout << "Mem: max p with lenth 3:  " << c.MemorizedCutRod(3) << endl;
    cout << "Mem: max p with lenth 6:  " << c.MemorizedCutRod(6) << endl;
    cout << "Mem: max p with lenth 10:  " << c.MemorizedCutRod(10) << endl;

    cout << "But: max p with lenth 1:  " << c.ButtomUpCutRod(1) << endl;
    cout << "But: max p with lenth 3:  " << c.ButtomUpCutRod(3) << endl;
    cout << "But: max p with lenth 6:  " << c.ButtomUpCutRod(6) << endl;
    cout << "But: max p with lenth 10:  " << c.ButtomUpCutRod(10) << endl;
    return 0;
}
