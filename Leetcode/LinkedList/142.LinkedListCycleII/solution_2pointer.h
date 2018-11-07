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
    ListNode *detectCycle(ListNode *head) {
        if(head == NULL) return NULL;
        ListNode* walker = head;
        ListNode* runner = head;
        bool iscycle = false;
        while(runner -> next != NULL && runner -> next -> next != NULL)
        {
            walker = walker -> next;
            runner = runner -> next -> next;
            if(walker == runner)
            {
                iscycle = true;
                break;
            }
        }
        if(!iscycle)
        {
            return NULL;
        }
        else
        {
            walker = head;
            while(walker != runner)
            {
                walker = walker -> next;
                runner = runner -> next;
            }
            return walker;
        }
        
    }
};
