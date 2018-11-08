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
    ListNode* removeElements(ListNode* head, int val) {
        while( head != NULL && head -> val == val) head = head -> next;
        if(head == NULL) return head;
        ListNode* cur = head;
        while(cur -> next != NULL)
        {
            if(cur -> next -> val == val)
            {
                ListNode* del = cur -> next;
                cur -> next = cur -> next -> next;
                delete del;
            }
            else
            {
                cur = cur -> next;
            }
        }
        return head;
    }
};
