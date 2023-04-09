#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <type_traits>
#include <memory>
#include <chrono>
#include <future>

class spin_mutex {
public:
  spin_mutex() : flag_(ATOMIC_FLAG_INIT) {}
  void lock() {
    while(flag_.test_and_set(std::memory_order_acquire)) {}
  }
  void unlock() {
    flag_.clear(std::memory_order_release);
  }
private:
  std::atomic_flag flag_;
};


int main() {
  {
    // is_lock_free
    struct A { int a[100]; };
    struct B { int a; };
    std::cout << "is_lock_free: atomic<bool>: " << std::atomic<bool>{}.is_lock_free()
              << " atomic<A>: " << std::atomic<A>{}.is_lock_free()
              << " atomic<B>: " << std::atomic<B>{}.is_lock_free() << std::endl;
    static_assert(std::atomic<int>::is_always_lock_free);
    static_assert(std::atomic<B>::is_always_lock_free);
    static_assert(!std::atomic<A>::is_always_lock_free);
  }
  {
    // atomic_flag
    std::atomic_flag af = ATOMIC_FLAG_INIT;
    auto race = [](std::atomic_flag& af, int n){
      for (int i = 0; i < n; ++i) {}
      if (!af.test_and_set()) {
        std::cout << std::this_thread::get_id() << " win!" << std::endl;
      }
    };
    std::vector<std::jthread> threads(10);
    for (auto&& thread : threads) {
      thread = std::jthread(race, std::ref(af), 50000);
    }
    spin_mutex m;
    int i = 1;
    auto lock_test = [&m, &i](int n){
      m.lock();
      for (int j = 0; j < n; ++j) {
        ++i;
      }
      m.unlock();
    };
    auto t1 = std::thread(lock_test, 10000);
    auto t2 = std::thread(lock_test, 10000);
    t1.join();
    std::cout << "i: " << i << std::endl;
    t2.join();
    std::cout << "i: " << i << std::endl;
  }
  {
    // basic interface
    std::atomic<bool> ab;
    ab = false;
    std::cout << ab; // 0
    ab.store(true);
    std::cout << " " << ab.load(); // 1
    std::cout << " " << ab.exchange(false) << " " << ab; // 1 0
    bool expected = true;
    while (!ab.compare_exchange_weak(expected /*expected*/, false /*desired*/) && expected) {
    }
    std::cout << " " << ab.load() << std::endl;; // 0

    // atomic<T*>
    struct Foo {
      void test() { std::cout << "Foo test." << std::endl; }
    };
    Foo f[5];
    std::atomic<Foo*> af(f);
    // XXX: fetch_add add size_t/ptrdiff_t and returns origin value
    std::cout << ((af.fetch_add(2) == f) && (af == (f + 2))) << std::endl;
    af.load()->test();

    // custom
    class A {
    public:
      constexpr virtual void f(){}
    };
    static_assert(!std::is_trivially_copyable<A>::value);
    // XXX: following failed on is_trivially_copyable check
    // std::atomic<A> custom_af;
    // XXX: c++20 allows shared_ptr as atomic template parameter:
    // https://en.cppreference.com/w/cpp/memory/shared_ptr/atomic2.
    // but it doesn't work with gcc 11
    // std::atomic<std::shared_ptr<int>> shared_ptr_atomic;
  }
  {
    // wait/notify_one in c++20
    // source: https://en.cppreference.com/w/cpp/atomic/atomic/wait
    // XXX: I don't think it's a valid example. Sleep main thread on
    // first time aysnc will cause the complete_jobs less than 20(given)
    // the solution is add a job_added sync point which indicates all jobs
    // are pushed into future_tasks;
    using namespace std::chrono_literals;
    std::atomic<bool> all_complete(false);
    std::atomic<bool> job_added(false);
    std::atomic<unsigned int> outstanding_jobs, completed_jobs;
    std::vector<std::future<void>> future_tasks(20);

    bool first_time = true;
    for (auto&& future_task : future_tasks) {
      ++outstanding_jobs;
      std::cout << outstanding_jobs << " " << std::this_thread::get_id() << std::endl;
      future_task = std::async([&](){
        ++completed_jobs;
        std::cout << "    " << outstanding_jobs << " " << std::this_thread::get_id() << std::endl;
        --outstanding_jobs;
        /* while(outstanding_jobs.load() != 0) {} */
        /* all_complete = true; */
        /* all_complete.notify_one(); */
        if (outstanding_jobs.load() == 0 && job_added.load()) {
          std::cout << std::this_thread::get_id() << std::endl;
          all_complete = true;
          all_complete.notify_one();
        }
      });

      if (first_time) {
        std::this_thread::sleep_for(100ms);
        first_time = false;
      }
    }
    // XXX: sync point. Still a potential issue is all future_task is finished before
    // job_added.store(true) is called. May cause deadlock. uncomment following line to reproduce.
    // std::this_thread::sleep_for(1000ms);
    job_added.store(true);

    all_complete.wait(false);
    std::cout << "completed_jobs: " << completed_jobs.load() << std::endl;
  }

}
