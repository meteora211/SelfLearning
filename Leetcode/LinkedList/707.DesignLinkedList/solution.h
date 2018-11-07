class MyLinkedList {
private:
    struct ListNode{
        int val;
        ListNode* prev;
        ListNode* next;
        ListNode(){};
        ListNode(int e, ListNode* p, ListNode* s):val(e),prev(p),next(s){};
    };
    ListNode* header = new ListNode();
    ListNode* tailer = new ListNode();
    int _size;
public:
    /** Initialize your data structure here. */
    MyLinkedList() {
        _size = 0;
        header -> next = tailer; tailer -> prev = header;
        header -> prev = NULL; tailer -> next = NULL;
    }
    
    /** Get the value of the index-th node in the linked list. If the index is invalid, return -1. */
    int get(int index) {
        ListNode* p = header;
        while(index-- >= 0)
        {
            if(tailer == (p = p -> next))
                return -1;
        }
        return p->val;
    }
    
    /** Add a node of value val before the first element of the linked list. After the insertion, the new node will be the first node of the linked list. */
    void addAtHead(int val) {
        ListNode* p = new ListNode();
        p -> val = val;
        p -> next = header -> next; p -> prev = header;
        header -> next -> prev = p; header -> next = p;
        _size ++;
    }
    
    /** Append a node of value val to the last element of the linked list. */
    void addAtTail(int val) {
        ListNode* p = new ListNode();
        p -> val = val;
        p -> prev = tailer -> prev; p -> next = tailer;
        tailer -> prev -> next = p; tailer -> prev = p;
        _size ++;
    }
    
    /** Add a node of value val before the index-th node in the linked list. If index equals to the length of linked list, the node will be appended to the end of linked list. If index is greater than the length, the node will not be inserted. */
    void addAtIndex(int index, int val) {
        if(index < 0 || index > _size) return;
        ListNode* p = new ListNode();
        ListNode* cur = header;
        while(tailer != cur && index >= 0)
        {
            cur = cur -> next;
            index --;
        }
        if(index >= 0)
        {
            return;
        }
        else
        {
            p -> val = val;
            p-> next = cur; p -> prev = cur -> prev;
            cur -> prev -> next = p; cur -> prev = p;
        }
        _size ++;
    }
    
    /** Delete the index-th node in the linked list, if the index is valid. */
    void deleteAtIndex(int index) {
        if(index < 0 || index > _size) return;
        ListNode* cur = header;
        while(tailer != cur && index>= 0)
        {
            cur = cur -> next;
            index --;
        }
        if(index >= 0 || cur == tailer)
        {
            return;
        }
        else
        {
            cur -> next -> prev = cur -> prev;
            cur -> prev -> next = cur -> next;
            delete cur;
        }
        _size --;
    }
};

/**
 * Your MyLinkedList object will be instantiated and called as such:
 * MyLinkedList obj = new MyLinkedList();
 * int param_1 = obj.get(index);
 * obj.addAtHead(val);
 * obj.addAtTail(val);
 * obj.addAtIndex(index,val);
 * obj.deleteAtIndex(index);
 */
