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
    ListNode* rotateRight(ListNode* head, int k) {
        if(head == NULL) return head;
        ListNode* cur = head;
        int size = 1;
        while(cur -> next != NULL)
        {
            size ++;
            cur = cur -> next;
        }
        cur -> next = head;
        k = k % size;
        int index = size - k;
        while(index--)
        {
            cur = cur -> next;
        }
        head = cur -> next;
        cur -> next = NULL;
        return head;
    }
};
