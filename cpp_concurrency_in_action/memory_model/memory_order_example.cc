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
      while(!x.load());
      if (y.load()) ++z; // 3
    };
    auto read_y_then_x = [&](){
      while(!y.load());
      if (x.load()) ++z; // 4
    };
    std::thread t1(write_x), t2(write_y), t3(read_x_then_y), t4(read_y_then_x);
    t1.join(); t2.join(); t3.join(); t4.join();
    // XXX: z must > 0;
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
      while(!x.load(std::memory_order_acquire));
      if (y.load(std::memory_order_acquire)) ++z; // 3
    };
    auto read_y_then_x = [&](){
      while(!y.load(std::memory_order_acquire));
      if (x.load(std::memory_order_acquire)) ++z; // 4
    };
    std::thread t1(write_x), t2(write_y), t3(read_x_then_y), t4(read_y_then_x);
    t1.join(); t2.join(); t3.join(); t4.join();
    // XXX: z can be zero
    std::cout << z.load() << std::endl;
  }
}
