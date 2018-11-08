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
    ListNode* oddEvenList(ListNode* head) {
        if(head == NULL || head -> next == NULL) return head;
        ListNode* cur = head -> next;
        ListNode* oddcur = head;
        while(cur -> next != NULL)
        {
            ListNode* temp = oddcur -> next;
            oddcur -> next = cur -> next;
            cur -> next = cur -> next -> next;
            oddcur -> next -> next = temp;
            oddcur = oddcur -> next;
            if(cur -> next != NULL)
                cur = cur ->next;
            else
                return head;
        }
        return head;
    }
};
