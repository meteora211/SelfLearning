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
    ListNode* reverseList(ListNode* head) {
        if(head == NULL || head -> next == NULL) return head;
        ListNode* cur = head;
        while(cur -> next != NULL)
        {
            ListNode* tempnext = cur -> next -> next;
            cur -> next -> next = head;
            head = cur -> next;
            cur -> next = tempnext;
        }
        return head;
    }
};
