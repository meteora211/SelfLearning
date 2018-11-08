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
    ListNode* reverseBetween(ListNode* head, int m, int n) {
        if(head == NULL) return head;
        int index = 0;
        ListNode* cur = new ListNode(0); cur -> next = head;
        ListNode* prehead = cur;
        while(cur != NULL && ++index < m)
        {
            cur = cur -> next;
        }
        ListNode* reversehead = cur;
        cur = cur -> next;
        if(index < m) return head;
        while(cur != NULL && cur -> next != NULL && index < n)
        {
            ListNode* temp = cur -> next;
            cur -> next = cur -> next -> next;
            temp -> next = reversehead -> next;
            reversehead -> next = temp;
            index ++;
        }
        return prehead->next;
    }
};
