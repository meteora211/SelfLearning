#include "thread_pool.h"

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
    // sleep for some times and check cpu utilization. It will raise to 100% due to yield between each threads.
    // std::this_thread::sleep_for(100s);
  }
  std::cout << "J: " << j << std::endl;
}
