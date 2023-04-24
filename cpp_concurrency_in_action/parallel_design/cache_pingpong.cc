#include <atomic>
#include <thread>
#include <latch>
#include <iostream>
#include <vector>
#include <chrono>
#include <ranges>
#include <algorithm>

using namespace std;
using namespace chrono;
using namespace std::chrono_literals;

void print_perf(int d) {
  vector<jthread> threads;
  atomic<size_t> n{0};
  atomic<int64_t> nsSum{0};
  const auto duration = chrono::microseconds(d);
  int hc = thread::hardware_concurrency();
  latch latSync(hc);
  for (auto thread_no = 0; thread_no < hc; ++thread_no) {
    threads.emplace_back([&](){
          latSync.arrive_and_wait();
          auto start = high_resolution_clock::now();
          while (n.fetch_add(1, memory_order::relaxed) < 100000 * hc) {
            this_thread::sleep_for(duration);
          }
          auto cost = duration_cast<microseconds>( high_resolution_clock::now() - start ).count();
          nsSum += cost;
        });
  }
  threads.resize(0);
  cout << hc << " with " << duration.count() << "microseconds. costSum: " << nsSum << " theory sum: " << 100000 * duration.count() * hc <<  " efficiency: " << (double)((nsSum  - (100000 * duration.count() * hc)) / (1.0e6* hc) ) << endl;
}

int main() {
  // I expect the larger sleeping time the smaller number is printed. But it doesn't.
  ranges::for_each(views::iota(1,100), print_perf);
}
