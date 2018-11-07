/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
class Solution {
public:
    ListNode *getIntersectionNode(ListNode *headA, ListNode *headB) {
        if(headA == NULL || headB == NULL) return NULL;
        int A = 1; int B = 1;
        ListNode* curA = headA; ListNode* curB = headB;
        while(curA -> next != NULL)
        {
            curA = curA -> next;
            A++;
        }
        while(curB -> next != NULL)
        {
            curB = curB -> next;
            B++;
        }
        curA = headA;
        curB = headB;
        if(A>=B)
        {
            while(A!=B)
            {
                curA = curA -> next;
                A --;
            }
        }
        else
        {
            while(A!=B)
            {
                curB = curB -> next;
                B --;
            }
        }
        while(A > 0)
        {
            if(curA == curB)
            {
                return curA;
            }
            else
            {
                curA = curA -> next; A--;
                curB = curB -> next; B--;
            }
        }
        return NULL;
    }
};
