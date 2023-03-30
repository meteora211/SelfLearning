#include <mutex>
#include <shared_mutex>
#include <thread>
#include <iostream>
#include <string>
#include <chrono>

class ThreadSafeCounter{
public:
  ThreadSafeCounter() = default;
  ~ThreadSafeCounter() = default;

  int get() const {
    std::shared_lock sl(mutex_);
    return x;
  }

  void increment() {
    std::unique_lock ul(mutex_);
    ++x;
  }

  void reset() {
    std::unique_lock ul(mutex_);
    x = 0;
  }

private:
  mutable std::shared_mutex mutex_;
  int x = 0;
};

class RecursiveIncrement {
public:
  RecursiveIncrement() = default;
  ~RecursiveIncrement() = default;

  void increment() {
    std::unique_lock ul(mutex_);
    ++x;
  }

  void recursive_increment(int num) {
    std::unique_lock ul(mutex_);
    if (num > 1) {
      recursive_increment(--num);
    }
    ++x;
  }

  int get() const {
    std::unique_lock ul(mutex_);
    return x;
  }

private:
  mutable std::recursive_mutex mutex_;
  int x = 0;
};

class OnlyOne {
public:
  OnlyOne() = default;
  ~OnlyOne() = default;

  void initialization() {
    std::call_once(flag_, [this]{init_impl();});
  }

  void init_impl() {
    std::cout << "actual init" << std::endl;
  }

  void call() {
    initialization();
    std::cout << "call" << std::endl;
  }
private:
  std::once_flag flag_;
};

int main() {
  ThreadSafeCounter c;
  auto fn = [&](){
    c.increment();
    std::cout << "thread_id: " << std::this_thread::get_id() << " get: " << c.get() << std::endl;
  };

  std::thread t1(fn);
  std::thread t2(fn);
  t1.join();
  t2.join();

  RecursiveIncrement r;
  auto f2 = [&](){
    r.recursive_increment(3);
    std::cout << "thread_id: " << std::this_thread::get_id() << " get: " << r.get() << std::endl;
  };
  std::thread t3(f2);
  std::thread t4(f2);
  t3.join();
  t4.join();

  OnlyOne o;
  std::thread t5(&OnlyOne::call, &o);
  std::thread t6(&OnlyOne::call, &o);
  t5.join();
  t6.join();
}
