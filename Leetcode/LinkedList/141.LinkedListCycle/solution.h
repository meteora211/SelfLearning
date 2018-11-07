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
    bool hasCycle(ListNode *head) {
        if(head == NULL) return false;
        ListNode* dyn = head;
        ListNode* sta = head;
        while(dyn -> next != NULL && dyn -> next -> next != NULL)
        {
            dyn = dyn -> next -> next;
            sta = sta -> next;
            if(dyn == sta) return true;
        }
        return false;
        
    }
};
