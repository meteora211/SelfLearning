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
    ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
        if(l1 == NULL) return l2;
        if(l2 == NULL) return l1;
        ListNode* newlist = new ListNode(0);
        ListNode* curlist = newlist;
        while(l1 != NULL && l2 != NULL)
        {
            int value;
            if(l1 -> val >= l2 -> val)
            {
                value = l2 -> val;
                l2 = l2 -> next;
            }
            else
            {
                value = l1 -> val;
                l1 = l1 -> next;
            }
            ListNode* temp = new ListNode(value);
            curlist -> next = temp;
            curlist = temp;
        }
        if(l1 != NULL) curlist -> next = l1;
        if(l2 != NULL) curlist -> next = l2;
        return newlist -> next;
    }
};
