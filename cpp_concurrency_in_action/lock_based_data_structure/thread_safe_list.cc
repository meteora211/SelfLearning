#include <mutex>
#include <memory>
#include <iostream>
#include <queue>
#include <condition_variable>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;


template<typename T>
class ThreadSafeList {
public:
  ThreadSafeList() = default;
  ~ThreadSafeList() {
    remove_if([](const Node&){return true;});
  };

  void push_head(const T& d) {
    auto node = std::make_unique<Node>(d);
    std::unique_lock l(head_.m);
    node->next = std::move(head_.next);
    head_.next = std::move(node);
  }

  template <typename F>
  std::shared_ptr<T> find_first_if(F f) {
    auto* cur = &head_;
    std::unique_lock head_lock(head_.m);
    while (const Node* next = cur->next.get()) {
      std::unique_lock next_lock(next.m);
      head_lock.unlock();
      if (f(*(next->data))) {
        return next->data;
      }
      cur = next;
      head_lock = std::move(next_lock);
    }
    return nullptr;
  }

  template <typename F>
  void remove_if(F f) {
    auto* cur = &head_;
    std::unique_lock head_lock(head_.m);
    while (const Node* next = cur->next.get()) {
      std::unique_lock next_lock(next.m);
      if (f(*(next->data))) {
        cur->next = std::move(next->next);
        next_lock.unlock();
      } else {
        head_lock.unlock();
        cur = next;
        head_lock = std::move(next_lock);
      }
    }
  }

  template <typename F>
  void for_each(F f) {
    auto* cur = &head_;
    std::unique_lock head_lock(head_.m);
    while (const Node* next = cur->next.get()) {
      std::unique_lock next_lock(next.m);
      head_lock.unlock();
      f(*(next->data));
      cur = next;
      head_lock = std::move(next_lock);
    }
  }

private:
  struct Node {
    Node() = default;
    Node(const T& d) : data(std::make_shared<T>(d)) {}
    std::mutex m;
    std::unique_ptr<Node> next;
    std::shared_ptr<T> data;
  };

  Node head_;
};
