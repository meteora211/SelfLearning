#include <concepts>
#include <coroutine>
#include <iostream>

template<typename T>
struct ReturnType {
  struct promise_type {
    ReturnType get_return_object() {
      return {.h_ = std::coroutine_handle<promise_type>::from_promise(*this)};
    }
    T initial_suspend() {
      std::cout << "[promise_type] initial_suspend." << std::endl;
      return {};
    }
    T final_suspend() noexcept {
      std::cout << "[promise_type] final_suspend." << std::endl;
      return {};
    }
    void unhandled_exception() {}
    void return_void() {}
  };
  std::coroutine_handle<promise_type> h_;
  void destroy() {
    h_.destroy();
  }
  void resume() {
    h_.resume();
  }
};

ReturnType<std::suspend_never > auto_hello_coroutine() {
  std::cout << "auto hello coroutine" << std::endl;
  co_return;
}

ReturnType<std::suspend_always > manual_hello_coroutine() {
  std::cout << "manual hello coroutine" << std::endl;
  co_return;
}

int main() {
  {
    // hello_coroutine
    auto_hello_coroutine();
  }
  {
    // hello_coroutine
    ReturnType c = manual_hello_coroutine();
    c.resume();
  }
}
