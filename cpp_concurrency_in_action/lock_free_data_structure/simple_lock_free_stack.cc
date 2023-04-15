#include <atomic>
#include <memory>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

template<typename T>
class SimpleStack {
public:
  SimpleStack() = default;
  ~SimpleStack() = default;

  void push(T& value) {
    auto new_node = std::make_shared<Node>(value);
    new_node->next = head_.load();
    while (!head_.compare_exchange_weak(new_node->next, new_node));
  }

  std::shared_ptr<T> pop() {
    auto old_head = head_.load();
    while (old_head && !head_.compare_exchange_weak(old_head, old_head->next)) {};
    auto res = old_head ? std::make_shared<T>(old_head->data) : nullptr;
    return res;
  }

  void print() {
    auto cur = head_.load();
    while (cur && head_.compare_exchange_weak(cur, cur->next)) {
      std::cout << cur->data << " -> ";
      cur = cur->next;
    }
    std::cout << std::endl;
  }

private:
  struct Node {
    Node(const T& d) : data(d) {}
    T data;
    std::shared_ptr<Node> next;
  };
  // XXX: Only available for g++12 to enable atomic<shared_ptr<T>>
  std::atomic<std::shared_ptr<Node>> head_;
};

int main() {
  SimpleStack<int> s;
  auto push_thread = [&](){
    for (int i = 0; i < 15; ++i) {
      s.push(i);
    }
  };

  auto pop_thread = [&](){
    std::this_thread::sleep_for(50ms);
    std::cout << "poping: ";
    for (int i = 0; i < 10; ++i) {
      std::cout << *(s.pop()) << " ";
    }
    std::cout << "end!"<< std::endl;
  };

  std::thread t1(push_thread);
  std::thread t2(push_thread);
  std::thread t3(pop_thread);

  t1.join(); t2.join(); t3.join();

  s.print();
}
