#include <algorithm>
#include <iostream>
#include <functional>
#include <numeric>
#include <thread>
#include <memory>
#include <vector>
#include <future>
#include <exception>

template<typename Iterator, typename T>
struct accumulate_block {
  void operator()(Iterator first, Iterator last, T& res) {
    res = std::accumulate(first, last, res);
    return;
  }
};

void throw_exception() {
  throw std::overflow_error("test");
}

template<typename Iterator, typename T>
T accumulate_parallel(Iterator first, Iterator last, T init) {
  size_t dist = std::distance(first, last);
  if (!dist) {
    return init;
  }

  std::size_t hardware_threads = std::thread::hardware_concurrency();
  auto block_size = dist / hardware_threads;

  std::vector<T> block_res;
  std::vector<std::thread> threads;
  std::vector<std::future<void>> futures;
  futures.resize(hardware_threads);
  block_res.resize(hardware_threads);
  auto start = first;
  for (size_t index = 0; index < hardware_threads; ++index) {
    auto end = start;
    std::advance(end, block_size);
    if (index == hardware_threads - 1) {
      end = last;
    }
    // XXX: OPTION1: create thread directly without exception safty.
    // if accumulate_block throw exception, main thread terminated. Enable throw_exception for test.
    // threads.emplace_back(accumulate_block<Iterator, T>{}, start, end, std::ref(block_res[index]));

    // XXX: OPTION2: create thread with packaged_task. but still facing the issue when
    // exception is throw before the thread is joined.
    // std::packaged_task<void(Iterator, Iterator, T&)> task(accumulate_block<Iterator, T>{});
    // futures[index] = task.get_future();
    // threads.emplace_back(std::move(task), start, end, std::ref(block_res[index]));

    // XXX: OPTION3
    futures[index] = std::async(accumulate_block<Iterator, T>{}, start, end, std::ref(block_res[index]));
    std::advance(start, block_size);
  }

  // OPTION1
  /* threads.emplace_back(throw_exception); */



  // OPTION2
  /* std::packaged_task<void()> task(throw_exception); */
  /* threads.emplace_back(std::move(task)); */
  /* std::for_each(threads.begin(), threads.end(), */
  /*               std::mem_fn(&std::thread::join)); */

  /* try { */
  /*   for (auto& f: futures) { */
  /*     f.get(); */
  /*   } */
  /* } catch (...) { */
  /*   for (auto& x : threads) { */
  /*     if (x.joinable()) { */
  /*       x.join(); */
  /*     } */
  /*   } */
  /*   throw; */
  /* } */

  // OPTION3
  // XXX: Still break the main thread. I don't think the solution in book make sense
  futures.push_back(std::async(std::launch::deferred, throw_exception));
  for (auto& f: futures) {
    try {
      f.get();
    } catch (const std::future_error& e) {
      std::cout << "Caught a future_error with code \"" << e.code()
                  << "\"\nMessage: \"" << e.what() << "\"\n";
    }
  }
  std::cout << "JOINED" << std::endl;


  accumulate_block<typename std::vector<T>::iterator, T>{}(block_res.begin(), block_res.end(), std::ref(init));
  return init;
}


int main() {
  std::vector<int> vec(100000);
  std::iota(vec.begin(), vec.end(), 0);
  auto res = accumulate_parallel(vec.begin(), vec.end(), int(0));
  auto ref = std::accumulate(vec.begin(), vec.end(), 0);
  std::cout << "res: " << res << " ref: " << ref << std::endl;
}
