## ListNode
```c++
template <typename T> struct ListNode{
    T Data;
    ListNode<T>* pred;  //pred指向的是前一个List Node，因此类型为ListNode<T>*而不是T*
    ListNode<T>* succ;
}
```

## List
### 头尾节点
头节点(header)和尾节点(trailer)始终存在，对外不可见
^header <-> first <---> last <-> trailer^

