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
        unordered_map<ListNode*, ListNode*> map;
        while(head != NULL)
        {
            if(map.find(head) != map.end())
                return head;
            else
                map[head] = head -> next;
            head = head -> next;
        }
        return NULL;
    }
};
