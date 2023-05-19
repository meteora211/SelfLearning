#include <atomic>
#include <thread>
#include <latch>
#include <iostream>
#include <vector>
#include <chrono>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <future>

using namespace std;
using namespace chrono;
using namespace std::chrono_literals;

template<typename Iterator, typename Func>
void parallel_for_each(Iterator first, Iterator last, Func f) {
  std::size_t dist = std::distance(first, last);
  if (!dist) return;

  std::size_t hardware_threads = std::thread::hardware_concurrency();
  auto block_size = dist / hardware_threads;

  auto start = first;
  std::vector<std::future<void>> futures;
  futures.resize(hardware_threads);
  for (size_t index = 0; index < hardware_threads; ++index) {
    auto end = start;
    std::advance(end, block_size);
    if (index == hardware_threads - 1) {
      end = last;
    }

    // use = in lambda. pass by ref cause random crash since start/end is not constant.
    // and changes in other thread.
    futures[index] = std::async([=](){std::for_each(start, end, f);});
    /* std::for_each(start, end, f); */
    std::advance(start, block_size);
  }

  for (auto&& f : futures) {
    f.get();
  }
}

template<typename Iterator, typename T>
Iterator simple_find(Iterator first, Iterator last, T value, std::atomic<bool>& stop_flag) {
  for (; first != last && !stop_flag; ++first) {
    if (*first == value) {
      stop_flag.store(true);
      return first;
    }
  }
  return last;
}

template<typename Iterator, typename T>
Iterator parallel_find(Iterator first, Iterator last, T value) {
  std::size_t dist = std::distance(first, last);
  if (!dist) return first;

  std::atomic<bool> stop_flag = false;

  std::size_t hardware_threads = std::thread::hardware_concurrency();
  auto block_size = dist / hardware_threads;

  auto start = first;
  std::vector<std::future<Iterator>> futures;
  futures.resize(hardware_threads);
  for (size_t index = 0; index < hardware_threads; ++index) {
    auto end = start;
    std::advance(end, block_size);
    if (index == hardware_threads - 1) {
      end = last;
    }

    // use = in lambda. pass by ref cause random crash since start/end is not constant.
    // and changes in other thread.
    futures[index] = std::async([=,&stop_flag](){
        auto res = simple_find(start, end, value, stop_flag);
        if (res == end) {
          res = last;
        }
        return res;
        });
    /* std::for_each(start, end, f); */
    std::advance(start, block_size);
  }

  for (auto&& f : futures) {
    auto res = f.get();
    if (res != last) {
      return res;
    }
  }

  return last;
}

int main() {
  vector<int> list(200);
  std::iota(list.begin(), list.end(), -100);
  parallel_for_each(list.begin(), list.end(), [](int& i) {i+= i;});
  /* std::for_each(list.begin(), list.end(), [](int& i) {i= i*i;}); */
  for (auto& i : list) {
    std::cout << i << " ";
  }
  std::cout << std::endl;

  auto res = parallel_find(list.begin(), list.end(), 12);
  auto ref = std::find(list.begin(), list.end(), 12);
  std::cout << *res << " " << *ref << std::endl;
}

