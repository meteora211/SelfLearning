#include <atomic>
#include <thread>
#include <iostream>

int main() {
  {
    // sequential consistency
    std::atomic<bool> x{false}, y{false};
    std::atomic<int> z;
    auto write_x = [&](){
      x.store(true); // 1
    };
    auto write_y = [&](){
      y.store(true); // 2
    };
    auto read_x_then_y = [&](){
      while(!x.load()); // 3
      if (y.load()) ++z; // 4
    };
    auto read_y_then_x = [&](){
      while(!y.load()); // 5
      if (x.load()) ++z; // 6
    };
    std::thread t1(write_x), t2(write_y), t3(read_x_then_y), t4(read_y_then_x);
    t1.join(); t2.join(); t3.join(); t4.join();
    // XXX: z must > 0;
    // 1 -> 3 -> 4
    // 2 -> 5 -> 6
    std::cout << z.load() << std::endl;
  }
  {
    // acquire release
    std::atomic<bool> x{false}, y{false};
    std::atomic<int> z;
    auto write_x = [&](){
      x.store(true, std::memory_order_release); // 1
    };
    auto write_y = [&](){
      y.store(true, std::memory_order_release); // 2
    };
    auto read_x_then_y = [&](){
      while(!x.load(std::memory_order_acquire)); // 3
      if (y.load(std::memory_order_acquire)) ++z; // 4
    };
    auto read_y_then_x = [&](){
      while(!y.load(std::memory_order_acquire)); // 5
      if (x.load(std::memory_order_acquire)) ++z; // 6
    };
    std::thread t1(write_x), t2(write_y), t3(read_x_then_y), t4(read_y_then_x);
    t1.join(); t2.join(); t3.join(); t4.join();
    // XXX: z can be zero
    std::cout << z.load() << std::endl;
  }
  {
    // relaxed
    std::atomic<bool> x, y;
    std::atomic<int> z;

    auto write_x_then_y = [&](){
      x.store(true, std::memory_order_relaxed); // 1
      y.store(true, std::memory_order_relaxed); // 2
    };
    auto read_y_then_x = [&](){
      while(!y.load(std::memory_order_relaxed)); // 3
      if (x.load(std::memory_order_relaxed)) ++z; // 4
    };
    std::thread t1(write_x_then_y), t2(read_y_then_x);
    t1.join(); t2.join();
    // XXX: z can be zero
    std::cout << z.load() << std::endl;
  }
  {
    // fence
    std::atomic<bool> x, y;
    std::atomic<int> z;

    auto write_x_then_y = [&](){
      x.store(true, std::memory_order_relaxed); // 1
      std::atomic_thread_fence(std::memory_order_release); // 2
      y.store(true, std::memory_order_relaxed); // 3
    };
    auto read_y_then_x = [&](){
      while(!y.load(std::memory_order_relaxed)); // 4
      std::atomic_thread_fence(std::memory_order_acquire); // 5
      if (x.load(std::memory_order_relaxed)) ++z; // 6
    };
    std::thread t1(write_x_then_y), t2(read_y_then_x);
    t1.join(); t2.join();
    // XXX: z > 0
    std::cout << z.load() << std::endl;
  }
}
