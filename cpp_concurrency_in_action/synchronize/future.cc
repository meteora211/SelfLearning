#include <concepts>
#include <string>
#include <thread>
#include <iostream>
#include <mutex>
#include <chrono>
#include <future>
#include <type_traits>
#include <vector>

template<typename T>
concept has_value_type = requires(T a){
  typename T::value_type;
};

template<typename T>
concept is_vector = has_value_type<T> && std::is_same_v<T, std::vector<typename T::value_type>>;

template <typename T>
void get(T& f) {
  std::cout << "get future: " << f.get() << std::endl;
}

template <>
void get<std::future<void>> (std::future<void>& f) {
  f.get();
  std::cout << "void future" << std::endl;
}

int find_answer() {
  return 42;
}

struct CallStruct {
  CallStruct() = default;
  ~CallStruct() = default;
  CallStruct(const CallStruct&) {
    std::cout << "copy ctor" << std::endl;
  }
  CallStruct(CallStruct&&) {
    std::cout << "move ctor" << std::endl;
  }
  CallStruct& operator=(const CallStruct&) {
    std::cout << "copy assign" << std::endl;
    return *this;
  }
  CallStruct& operator=(CallStruct&&) {
    std::cout << "move assign" << std::endl;
    return *this;
  }

  void foo() {
    std::cout << "foo" << std::endl;
  }
  std::string bar(const std::string& b) {
    std::cout << "bar: " << b << std::endl;
    return b;
  }

};

int main() {
  // basic usage
  {
    std::future<int> the_answer = std::async(find_answer);
    std::cout << the_answer.get() << std::endl;
  }

  // call struct method
  {
    CallStruct cs;
    auto foo_ref = std::async(&CallStruct::foo, &cs);
    auto foo_copy = std::async(&CallStruct::foo, cs);
    auto foo_move = std::async(&CallStruct::foo, CallStruct{});
    get(foo_ref); get(foo_copy); get(foo_move);
    auto bar_ref = std::async(&CallStruct::bar, &cs, "ref");
    auto bar_copy = std::async(&CallStruct::bar, cs, "copy");
    auto bar_move = std::async(&CallStruct::bar, CallStruct{}, "move");
    get(bar_ref); get(bar_copy); get(bar_move);
  }

  // future methods
  {
    // valid
    std::future<int> f1, f2;
    std::cout << f1.valid() << f2.valid() << std::endl;
  }
  {
    // move constructor for future
    f1 = std::async(find_answer);
    // XXX: future is move only
    // f2 = f1;
    f2 = std::move(f1);
    std::cout << f1.valid() << f2.valid() << std::endl;
    get(f2);
  }
  {
    // shared_future
    f1 = std::async(find_answer);
    std::shared_future<int> f3 = f1.share();
    std::cout << f1.valid() << f2.valid() << f3.valid() << std::endl;
    // XXX: shared_future can get multiple times
    get(f3);
    get(f3);
  }

  {
    // wait
    auto is_prime = [](int n) {
      for (int i = 2; i < n; ++i) if (n%i == 0) return true;
      return false;
    };
    auto f4 = std::async(is_prime, 194232491);
    f4.wait();
    get(f4);
  }

  static_assert(is_vector<std::vector<int>>);
  static_assert(!is_vector<std::string>);


}
