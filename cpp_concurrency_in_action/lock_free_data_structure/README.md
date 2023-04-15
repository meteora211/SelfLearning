### pop implementation

原书上的实现通过引用计数来确保节点`std::atomic<Node*> head_` 的释放。考虑`std::atomic<Node*>`类型的pop代码：
```
  std::shared_ptr<T> pop() {
    auto old_head = head_.load();
    while (old_head && !head_.compare_exchange_weak(old_head, old_head->next)) {};
    auto res = old_head ? std::make_shared<T>(old_head->data) : nullptr;
    return res;
  }
```
当多个线程同时进行pop操作时，pop的节点如何删除？ 考虑如下代码：

```
  std::shared_ptr<T> pop() {
    Node* t = head_.load();
    while (t && !head_.compare_exchange_weak(t, t->next));
    std::shared_ptr<T> res;
    if (t) {
      res.swap(t->v);
    }
    try_delete(t); // delete Node here
    return res;
  }
```

假设线程A在`try_delete(t)` 中尝试删除节点时，如果有线程B使用到节点t，可能会引发data race。解决方式就是引入引用计数，确保在delete过程中仅有当前线程在访问该节点。

而这些复杂的实现在c++20之后都不是问题，因为atomic支持shared_ptr特化：https://en.cppreference.com/w/cpp/memory/shared_ptr/atomic2
