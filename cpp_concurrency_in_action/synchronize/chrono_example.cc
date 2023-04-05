#include <chrono>
#include <thread>
#include <future>
#include <iostream>
#include <condition_variable>

std::condition_variable cv;
std::mutex m;

using namespace std::chrono_literals;
using namespace std::chrono;

int some_task() {
  std::this_thread::sleep_for(30ms);
  return 42;
}



int main() {

  {
    // basic usage
    milliseconds ms(12345);
    seconds s = duration_cast<seconds>(ms);
    std::cout << "time count: " << s.count() << std::endl;
  }

  {
    auto start = high_resolution_clock::now();
    some_task();
    auto end = high_resolution_clock::now();
    // XXX: ERROR in book, second template parameter should be ratio<>
    // std::chrono::seconds represent a duration type
    // auto d  = duration<double, seconds>(end - start).count();
    duration<double, std::milli> dms  = end - start;
    duration<double> ds  = end - start;
    std::cout << ds.count() << " " << dms.count() << std::endl;
  }

  {
    // future wait for
    auto f = std::async(std::launch::async, some_task);
    if (f.wait_for(35ms) == std::future_status::ready) {
      std::cout << "wait success" << std::endl;
      std::cout << f.get() << std::endl;
    }
  }

  {
    // cv wait for
    bool task_done = false;
    auto f1 = [&](){
      std::unique_lock l(m);
      some_task();
      task_done = true;
      cv.notify_one();
    };
    auto f2 = [&](){
      std::unique_lock l(m);
      while (!task_done) {
        cv.wait_for(l, 35ms);
      }
      std::cout << "wait success" << std::endl;
    };
    auto t1 = std::thread(f1);
    auto t2 = std::thread(f2);
    t1.join();
    t2.join();
  }

}
