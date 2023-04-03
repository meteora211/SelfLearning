#include <iostream>
#include <deque>
#include <mutex>
#include <thread>
#include <future>
#include <functional>
#include <type_traits>

struct TaskHandler {
  TaskHandler()  {
    background_thread = std::thread([&]{this->background_task();});
  };

  ~TaskHandler() {
    stop_=true;
    background_thread.join();
  };

  /* void init() { */
  /*   background_thread(&TaskHandler<Callable>::background_task, this); */
  /* } */

  void background_task() {
    while (!stop_) {
      if (!deque_var.empty()) {
        std::unique_lock<std::mutex> lc(m_);
        /* auto&& task = std::move(deque_var.front()); */
        auto task = deque_var.front();
        std::cout << "executing task in: " << std::this_thread::get_id() << std::endl;
        task();
        deque_var.pop_front();
      }
    }
  }

  template<typename Callable, typename... Args>
  auto add_task(Callable&& callable, Args&&... args) {
    // XXX: https://stackoverflow.com/questions/43018646/compile-error-when-trying-to-use-stdresult-of
    using return_type = typename std::result_of<Callable(Args...)>::type;
    auto task = std::packaged_task<return_type()>(std::bind(std::forward<Callable>(callable), std::forward<Args>(args)...));
    /* auto task = std::packaged_task<return_type()>( */
    /*                 std::bind(std::forward<F>(f), std::forward<Args>(args)...)); */
    auto handle = task.get_future();
    deque_var.emplace_back([task](){ task(); });
    std::unique_lock<std::mutex> lc(m_);
    return handle;
  }


  bool stop_ = false;
  mutable std::mutex m_;
  std::deque<std::function<void()>> deque_var;
  std::thread background_thread;
};


int main() {
  TaskHandler th;
  std::cout << "main thread: " << std::this_thread::get_id() << std::endl;
  auto f1 = th.add_task([](){return 42;});
  auto f2 = th.add_task([](){return 100;});
  auto f3 = th.add_task([](){return 66;});

  std::cout << f1.get() << std::endl;
  std::cout << f2.get() << std::endl;
  std::cout << f3.get() << std::endl;

  auto is_prime = [](int n) {
    for (int i = 2; i < n; ++i) if (n%i == 0) return true;
    return false;
  };

  TaskHandler is_prime_handler;
  int large_int = 194232491;
  const int large_int_const = 194232491;
  auto f4 = is_prime_handler.add_task(is_prime, large_int);
  auto f5 = is_prime_handler.add_task(is_prime, large_int_const);
  auto f6 = is_prime_handler.add_task(is_prime, 194232491);

  std::cout << f4.get() << std::endl;
  std::cout << f5.get() << std::endl;
  std::cout << f6.get() << std::endl;
}
