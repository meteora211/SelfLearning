#include <cstddef> // Define NULL and size_t ...
#include <cstdlib> // Define NULL, exit, malloc, free ...

struct ListNode{
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}  //初始化列表 Initilizaition List
};


class Solution{
public:
ListNode* addTwoNumbers(ListNode* l1, ListNode* l2)
{
    ListNode prehead(0);
    ListNode *p = &prehead;
    int extra = 0;
    while(l1||l2||extra)
    {
        int sum = (l1? l1 -> val : 0)+(l2 ? l2 -> val : 0)+extra;
        extra = sum / 10;
        sum = sum % 10;
        p -> next = new ListNode (sum);
        p = p -> next;
        l1 = l1 ? l1 -> next : NULL;
        l2 = l2 ? l2 -> next : NULL;
    }
    return prehead.next;
}
};
