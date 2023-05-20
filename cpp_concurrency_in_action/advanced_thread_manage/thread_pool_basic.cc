#include <mutex>
#include <iostream>
#include <queue>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <atomic>
#include <functional>

using namespace std::chrono_literals;

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

class thread_pool_basic {
public:
  thread_pool_basic() {
    std::size_t n = std::thread::hardware_concurrency();
    // catch exception: set done_ to true and join all threads_;
    try {
      for (int i = 0; i < n; ++i) {
        threads_.emplace_back(&thread_pool_basic::work_thread, this);
      }
    } catch (...) {
      done_ = true;
      for (auto& t : threads_) {
        if (t.joinable()) {
          t.join();
        }
      }
    }
  }

  ~thread_pool_basic() {
    done_ = true;
    for (auto& t : threads_) {
      if (t.joinable()) {
        t.join();
      }
    }
  }

  void work_thread() {
    // XXX: without empty check, the worker thread skip the tasks remains in queue_ when done is set to true.
    while(!done_ || !queue_.empty()) {
      std::function<void()> task;
      if (queue_.try_pop(task)) {
        task();
      } else {
        std::this_thread::yield;
      }
    }
  }

  template<typename F>
  void submit(F task) {
    queue_.push(std::function<void()>(task));
  }

  size_t size() {
    return queue_.size();
  }

private:
  std::atomic<bool> done_;
  ThreadSafeQueue<std::function<void()>> queue_;
  std::vector<std::jthread> threads_;
};

int main() {
  int j = 0;
  {
    thread_pool_basic tpb;
    int i = 0;
    std::mutex m;
    for (; i < 100; ++i) {
      auto fn = [&, i](){
        ++j;
        std::this_thread::sleep_for(10ms);
        std::lock_guard lk(m);
        std::cout << "printing i: " << i << " in thread id: " << std::this_thread::get_id() << std::endl;
      };
      tpb.submit(fn);
    }

  }
  std::cout << "J: " << j << std::endl;
}
