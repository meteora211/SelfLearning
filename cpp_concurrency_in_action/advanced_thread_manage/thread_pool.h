#include <mutex>
#include <iostream>
#include <queue>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <atomic>
#include <functional>
#include <memory>
#include <future>

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

  void push(T&& elem) {
    {
      std::unique_lock l(m_);
      data_.push(std::forward<T>(elem));
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

class function_wrapper {
public:
  function_wrapper() = default;
  function_wrapper(const function_wrapper&) = delete;
  // XXX: note that no CONST in rvalue
  function_wrapper(function_wrapper&& rhs) noexcept : impl_(std::move(rhs.impl_)) {}
  function_wrapper& operator=(const function_wrapper&) = delete;
  function_wrapper& operator=(function_wrapper&& rhs) {
    impl_ = std::move(rhs.impl_);
    return *this;
  }
  template<typename F>
  function_wrapper(F&& f) : impl_(std::make_unique<caller<F>>(std::move(f))) {}
  void operator()() const { impl_->call(); }

private:
  // XXX: trick here is caller_base derives a template class caller<T>
  // so impl_ could be declared with unique_ptr<caller_base> without
  // an template arguments which is actually unkown.
  struct caller_base {
    virtual ~caller_base() = default;
    virtual void call() = 0;
  };
  template<typename F>
  struct caller : public caller_base {
    caller(F&& f): fn_(std::move(f)) {}
    void call() override final {
      fn_();
    }
    F fn_;
  };
  std::unique_ptr<caller_base> impl_;
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


class thread_pool_with_wrapper {
public:
  thread_pool_with_wrapper() {
    std::size_t n = std::thread::hardware_concurrency();
    try {
      for (int i = 0; i < n; ++i) {
        threads_.emplace_back(&thread_pool_with_wrapper::work_thread, this);
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

  ~thread_pool_with_wrapper() {
    done_ = true;
    for (auto& t : threads_) {
      if (t.joinable()) {
        t.join();
      }
    }
  }

  void work_thread() {
    while(!done_ || !queue_.empty()) {
      // XXX: changes to function_wrapper from std::function<void()>;
      function_wrapper task;
      if (queue_.try_pop(task)) {
        task();
      } else {
        std::this_thread::yield;
      }
    }
  }

  template<typename F>
  std::future<std::invoke_result_t<F>> submit(F f) {
    std::packaged_task<std::invoke_result_t<F>()> task(std::move(f));
    auto future = task.get_future();
    queue_.push(std::move(task));
    return future;
  }

  size_t size() {
    return queue_.size();
  }

private:
  std::atomic<bool> done_;
  ThreadSafeQueue<function_wrapper> queue_;
  std::vector<std::jthread> threads_;
};

class thread_pool_local_queue {
public:
  thread_pool_local_queue() {
    std::size_t n = std::thread::hardware_concurrency();
    try {
      for (int i = 0; i < n; ++i) {
        threads_.emplace_back(&thread_pool_local_queue::work_thread, this);
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

  ~thread_pool_local_queue() {
    done_ = true;
    for (auto& t : threads_) {
      if (t.joinable()) {
        t.join();
      }
    }
  }

  void work_thread() {
    local_queue_ = std::make_unique<std::queue<function_wrapper>>();
    std::cout << "DEBUG: RESET " << local_queue_.get() << " IN " << std::this_thread::get_id() << std::endl;
    bool local_task = local_queue_ && !(local_queue_->empty());
    while(!done_ || !queue_.empty() || local_task) {
      // XXX: changes to function_wrapper from std::function<void()>;
      function_wrapper task;
      if (local_task) {
        std::cout << "DEBUG: USE " << local_queue_.get() << " IN " << std::this_thread::get_id() << std::endl;
        task = std::move(local_queue_->front());
        local_queue_->pop();
      } else if (queue_.try_pop(task)) {
        task();
      } else {
        std::this_thread::yield;
      }
      local_task = local_queue_ && !(local_queue_->empty());
    }
  }

  template<typename F>
  std::future<std::invoke_result_t<F>> submit(F f) {
    std::packaged_task<std::invoke_result_t<F>()> task(std::move(f));
    auto future = task.get_future();
    if (local_queue_) {
      std::cout << "DEBUG: PUSH " << local_queue_.get() << " IN " << std::this_thread::get_id() << std::endl;
      local_queue_->push(std::move(task));
    } else {
      queue_.push(std::move(task));
    }
    return future;
  }

  size_t size() {
    return queue_.size();
  }

private:
  std::atomic<bool> done_;
  ThreadSafeQueue<function_wrapper> queue_;
  std::vector<std::jthread> threads_;
  // XXX: Names of thread_local variables with internal or external linkage referred from different scopes may refer to the same or to different instances depending on whether the code is executing in the same or in different threads.
  static thread_local std::unique_ptr<std::queue<function_wrapper>> local_queue_;
};

thread_local std::unique_ptr<std::queue<function_wrapper>> thread_pool_local_queue::local_queue_;

class steal_queue {
public:
  steal_queue() = default;
  steal_queue(const steal_queue&) = delete;
  steal_queue& operator=(const steal_queue&) = delete;

  void push(function_wrapper task) {
    std::unique_lock l(m_);
    data_.push_front(std::move(task));
  }

  bool empty() const {
    std::unique_lock l(m_);
    return data_.empty();
  }

  bool try_pop(function_wrapper& task) {
    std::unique_lock l(m_);
    if (data_.empty()) {
      return false;
    }
    task = std::move(data_.front());
    data_.pop_front();
    return true;
  }

  bool try_steal(function_wrapper& task) {
    std::unique_lock l(m_);
    if (data_.empty()) {
      return false;
    }
    task = std::move(data_.back());
    data_.pop_back();
    return true;
  }

private:
  std::deque<function_wrapper> data_;
  mutable std::mutex m_;
};


class thread_pool_steal_queue {
public:
  thread_pool_steal_queue() {
    std::size_t n = std::thread::hardware_concurrency();
    try {
      for (int i = 0; i < n; ++i) {
        work_stealing_queue_.emplace_back(std::make_unique<steal_queue>());
        threads_.emplace_back(&thread_pool_steal_queue::work_thread, this, i);
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

  ~thread_pool_steal_queue() {
    done_ = true;
    for (auto& t : threads_) {
      if (t.joinable()) {
        t.join();
      }
    }
  }

  void work_thread(int index) {
    index_ = index;
    local_queue_ = work_stealing_queue_[index_].get();
    std::cout << "DEBUG: GET " << local_queue_ << " IN " << std::this_thread::get_id() << std::endl;
    while(!done_) {
      // XXX: changes to function_wrapper from std::function<void()>;
      function_wrapper task;
      if (pop_task_from_queue(task) || pop_task_from_local_queue(task) || steal_task_from_other_thread(task)) {
        task();
      } else {
        std::this_thread::yield;
      }
    }
  }

  template<typename F>
  std::future<std::invoke_result_t<F>> submit(F f) {
    std::packaged_task<std::invoke_result_t<F>()> task(std::move(f));
    auto future = task.get_future();
    if (local_queue_) {
      std::cout << "DEBUG: PUSH " << local_queue_ << " IN " << std::this_thread::get_id() << std::endl;
      local_queue_->push(std::move(task));
    } else {
      queue_.push(std::move(task));
    }
    return future;
  }

  size_t size() {
    return queue_.size();
  }

private:
  bool pop_task_from_local_queue(function_wrapper& task) {
    return local_queue_ && local_queue_->try_pop(task);
  }

  bool pop_task_from_queue(function_wrapper& task) {
    return queue_.try_pop(task);
  }

  bool steal_task_from_other_thread(function_wrapper& task) {
    for (int i = 0; i < work_stealing_queue_.size(); ++i) {
      std::size_t index = (index_ + i + 1) % work_stealing_queue_.size();
      if (work_stealing_queue_[i]->try_steal(task)) {
        return true;
      }
    }
    return false;
  }
  std::atomic<bool> done_;
  ThreadSafeQueue<function_wrapper> queue_;
  std::vector<std::jthread> threads_;
  std::vector<std::unique_ptr<steal_queue>> work_stealing_queue_;
  // XXX: Names of thread_local variables with internal or external linkage referred from different scopes may refer to the same or to different instances depending on whether the code is executing in the same or in different threads.
  static thread_local steal_queue* local_queue_;
  static thread_local std::size_t index_;
};

thread_local steal_queue* thread_pool_steal_queue::local_queue_;
thread_local std::size_t thread_pool_steal_queue::index_;
