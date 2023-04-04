#include <iostream>
#include <thread>
#include <future>
#include <chrono>
using namespace std::chrono_literals;

void basic_usage(std::promise<int>& p) {
  auto f = p.get_future();
  std::cout << "getting future" << std::endl;
  std::cout << f.get() << std::endl;
}

void catch_exception(std::future<int>& f) {
  try {
    std::cout << f.get();
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}

int main() {
  {
    // basic usage
    auto p = std::promise<int>();
    auto t = std::thread(basic_usage, std::ref(p));
    std::this_thread::sleep_for(10ms);
    std::cout << "set value" << std::endl;
    p.set_value(42);
    t.join();
  }
  {
    // set exception
    auto p = std::promise<int>();
    auto f = p.get_future();
    auto t = std::thread(catch_exception, std::ref(f));
    p.set_exception(std::make_exception_ptr(std::runtime_error("custom runtime error")));
    t.join();
  }
}
