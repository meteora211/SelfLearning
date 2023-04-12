#include <mutex>
#include <iostream>
#include <queue>
#include <condition_variable>
#include <thread>

template<typename T>
class ThreadSafeQueue {
public:
  ThreadSafeQueue() = default;
  ThreadSafeQueue(const ThreadSafeQueue& rhs) {
    std::unique_lock l(rhs.m_);
    data_ = rhs.data_;
  }
  explicit ThreadSafeQueue(const std::queue<T>& data) : data_(data) {}

  ~ThreadSafeQueue() = default;

  void push(T elem) {
    {
      std::unique_lock l(m_);
      data_.push(elem);
      // XXX: notify_one within a lock
      // It's okay and no error, but might have a little bit perf drawback
      // compare to outside a lock
      // cv_.notify_one();
    }
    cv_.notify_one();
  }

  void wait_pop(T& value) {
    std::unique_lock l(m_);
    cv_.wait(l, [this]{return !data_.empty();});
    value = std::move(data_.front());
    data_.pop();
  }

  bool try_pop(T& value) {
    std::unique_lock l(m_);
    if (data_.empty()) return false;
    value = std::move(data_.front());
    data_.pop();
    return true;
  }

  bool empty() const {
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

  size_t size() const {
    std::unique_lock l(m_);
    return data_.size();
  }

private:
  std::queue<T> data_;
  mutable std::mutex m_;
  std::condition_variable cv_;
};

int main() {
  ThreadSafeQueue<int> queue;
  auto producer = [&](){
    for (int i = 0; i < 10; ++i) {
      queue.push(i);
      std::cout << "size: " << queue.size() << " push: " << i << std::endl;
    }
  };

  auto consumer = [&](){
    for (int i = 0; i < 10; ++i) {
      int value;
      queue.wait_pop(value);
      std::cout << "size: " << queue.size() << " pop: " << value << std::endl;
    }
  };

  std::thread t1(producer);
  std::thread t2(consumer);

  t1.join();
  t2.join();
}
