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
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        if(n <= 0 || head == NULL) return head;
        ListNode* cur = head;
        ListNode* removelist = head;
        while(n--)
        {
            if(cur -> next == NULL)
            {
                return head -> next;
            }
            else
            {
                cur = cur -> next;
            }
            
        }
        while(cur -> next != NULL)
        {
            cur = cur -> next;
            removelist = removelist -> next;
        }
        removelist -> next = removelist -> next -> next;
        return head;
    }
};
