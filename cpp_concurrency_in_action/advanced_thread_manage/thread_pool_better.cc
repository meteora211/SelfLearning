#include <future>
#include <vector>
#include <thread>
#include <queue>
#include <iostream>
#include <type_traits>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <memory>

class thread_pool {
public:
  thread_pool(size_t size = 0) : size_(size), done_(false) {
    if (size = 0) {
      size_ = std::thread::hardware_concurrency();
    }
    for (int i = 0; i < size_; ++i) {
      workers_.emplace_back(&thread_pool::worker_thread, this);
    }
  }
  ~thread_pool() {
    // XXX: add lock for atomic done_?
    done_ = true;
    cv_.notify_all();
    for (auto&& worker : workers_) {
      if (worker.joinable()) {
        worker.join();
      }
    }
  }

  void worker_thread() {
    std::function<void()> task;
    while(true) {
      {
        std::unique_lock lock(m_);
        cv_.wait(lock, [&](){return done_ || !tasks_.empty();});
        if (done_ && tasks_.empty()) {
          return;
        }
        task = std::move(tasks_.front());
        tasks_.pop();
      }
      task();
    }
  }

  template <typename F, typename... Args>
  auto submit(F&& f, Args&&... args) {
    using result_type = std::invoke_result_t<F, Args...>;
    auto task = std::make_shared<std::packaged_task<result_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
    auto future = task->get_future();
    {
      std::unique_lock lock(m_);
      tasks_.emplace([task](){(*task)();});
    }
    cv_.notify_one();
    return future;
  }

private:
  std::size_t size_;
  std::atomic<bool> done_;
  std::mutex m_;
  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> tasks_;
  std::condition_variable cv_;
};

int main() {
  auto f1 = [](double d){
    return d * d;
  };
  auto f2 = [](int a, int b){
    return a + b;
  };
  std::vector<std::future<double>> vec1;
  std::vector<std::future<int>> vec2;
  {
    thread_pool p(25);
    for (int i = 0; i < 100; ++i) {
      if (i % 2 == 0) {
        vec1.push_back(p.submit(f1, static_cast<double>(i)));
      } else {
        vec2.push_back(p.submit(f2, i - 1, i));
      }
    }
  }
  for (auto&& f : vec1) {
    std::cout << f.get() << std::endl;
  }
  for (auto&& f : vec2) {
    std::cout << f.get() << std::endl;
  }
}
