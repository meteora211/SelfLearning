#include <mutex>
#include <iostream>
#include <stack>
#include <condition_variable>
#include <thread>

template<typename T>
class ThreadSafeStack {
public:
  ThreadSafeStack() = default;
  ThreadSafeStack(const ThreadSafeStack& rhs) {
    std::unique_lock l(rhs.m_);
    data_ = rhs.data_;
  }
  explicit ThreadSafeStack(const std::stack<T>& data) : data_(data) {}

  ~ThreadSafeStack() = default;

  void push(T elem) {
    std::unique_lock l(m_);
    data_.push(elem);
  }

  void pop(T& value) {
    std::unique_lock l(m_);
    if (data_.empty()) throw;
    // https://stackoverflow.com/questions/37894068/assigning-rvalue-reference-to-lvalue-reference
    value = std::move(data_.top());
    data_.pop();
  }

  bool empty() {
    std::unique_lock l(m_);
    return data_.empty();
  }

  void clear_and_print() {
    std::unique_lock l(m_);
    while (!data_.empty()) {
      auto&& v = data_.top();
      std::cout << v << " ";
      data_.pop();
    }
    std::cout << std::endl;
  }

private:
  std::stack<T> data_;
  mutable std::mutex m_;
};

int main() {
  std::stack<int> tmp;
  std::mutex m;
  bool done = false;
  for (int i = 0; i < 10; ++i) tmp.push(i);
  ThreadSafeStack<int> s1(tmp);
  ThreadSafeStack<int> s2;
  auto pop_thread = [&](){
    while(true) {
      {
        std::unique_lock l(m);
        if (done && s1.empty()) break;
      }
      if (s1.empty()) continue;
      int value;
      s1.pop(value);
      s2.push(value);
    }
  };
  auto push_thread = [&](){
    for (int i = 0; i < 10; ++i) s1.push(i);
    {
      std::unique_lock l(m);
      done = true;
    }
  };

  std::thread t1(pop_thread);
  std::thread t2(push_thread);
  t1.join(); t2.join();

  s1.clear_and_print();
  s2.clear_and_print();

}
