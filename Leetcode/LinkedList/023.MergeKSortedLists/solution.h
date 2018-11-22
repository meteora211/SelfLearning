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
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        if(lists.empty()) return NULL;
        int len = lists.size();
        int index = 0;
        ListNode* header = new ListNode(0);
        ListNode* cur = header;
        while(index < len)
        {
            ListNode* curlist = lists[index++];
            while(curlist != NULL)
            {
                if(cur -> next == NULL) 
                {
                    cur -> next = new ListNode(curlist -> val);
                    cur = cur -> next;
                    curlist = curlist -> next;
                    continue;
                }
                if(cur -> next -> val >= curlist -> val)
                {
                    ListNode* temp = new ListNode(curlist -> val);
                    temp -> next = cur -> next;
                    cur -> next = temp;
                    curlist = curlist -> next;
                }
                cur = cur -> next; 
            }
            cur = header;
        }
        return header -> next;
    }
};
