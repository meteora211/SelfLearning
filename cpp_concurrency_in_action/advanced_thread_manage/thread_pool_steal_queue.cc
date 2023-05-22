#include "thread_pool.h"

int main() {
  int j = 0;
  std::vector<std::future<int>> futures;
  {
    thread_pool_steal_queue tps;
    int i = 0;
    std::mutex m;
    for (; i < 100; ++i) {
      auto fn = [&, i](){
        std::lock_guard l(m);
        ++j;
        std::cout << "printing i: " << i << " in thread id: " << std::this_thread::get_id() << std::endl;
        return i;
      };
      futures.emplace_back(tps.submit(fn));
    }
    // sleep for some times and check cpu utilization. It will raise to 100% due to yield between each threads.
    // std::this_thread::sleep_for(100s);
  }
  std::cout << "J: " << j << std::endl;
  for (auto&& f : futures) {
    std::cout << "returns: " << f.get() << std::endl;
  }
}
