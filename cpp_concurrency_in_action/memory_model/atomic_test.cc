#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

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

  }

}
