## Notes:
要着重考虑下一个节点是否为NULL的情况
```c++
ListNode* head;
head -> next = balabala;
```
比如以上这种情况就会报错，如何默认初始化节点然后设置节点的next？
