#include <mutex>
#include <thread>
#include <iostream>
#include <vector>
#include <atomic>
#include <future>
#include <chrono>
using namespace std::chrono_literals;

int main() {
  {
    std::mutex m;
    std::vector<std::future<void>> v(10);
    // lock only
    auto lock_only = [&](){
      m.lock();
      std::cout << std::this_thread::get_id() << " locked!" << std::endl;
      // dead lock here without unlock
      m.unlock();
    };

    for (auto&& f : v) {
      f = std::async(lock_only);
    }

    for (auto&& f : v) {
      if (f.wait_for(50ms) == std::future_status::timeout) {
        std::cout << "timeout!" << std::endl;
      }
    }
  }
  {
    // atomic acquire only
    std::vector<std::thread> v(10);
    std::atomic<int> a(10);
    auto lock_only = [&](){
      std::cout << std::this_thread::get_id() << " load: " << a.load(std::memory_order_acquire) << std::endl;
    };
    for (auto&& t : v) {
      t = std::thread(lock_only);
    }

    for (auto&& t : v) {
      t.join();
    }
    std::cout << "Final a: " << a.load() << std::endl;

  }
}
