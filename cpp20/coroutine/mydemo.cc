#include <concepts>
#include <coroutine>
#include <iostream>
#include <string>
struct pretty_print {
  pretty_print(const std::string& name) : name_(name) {
    std::cout << "==================" << name_ << " start====================" << std::endl;
  }
  ~pretty_print() {
    std::cout << "==================" << name_ << " end====================" << std::endl;
  }
  std::string name_;
};

template<typename T = std::suspend_never>
struct ReturnType {
  struct promise_type {
    int answer;
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
  int get_the_answer() {
    std::cout << "Get it!" << std::endl;
    return h_.promise().answer;
  }
};

template<typename Promise>
struct Awaitable {
  Awaitable (int answer) : answer_(answer) {}
  bool await_ready() const noexcept {
    std::cout << "[Awaiter] In await_ready function\n";
    return false;
  }
  void await_suspend(std::coroutine_handle<Promise> h) {
    std::cout << "Found the answer" << std::endl;
    h.promise().answer = answer_;
  }
  void await_resume() const noexcept {
    std::cout << "[Awaiter] In await_resume function\n";
  }

  int answer_;
};

ReturnType<std::suspend_never > auto_hello_coroutine() {
  std::cout << "auto hello coroutine" << std::endl;
  co_return;
}

ReturnType<std::suspend_always > manual_hello_coroutine() {
  std::cout << "manual hello coroutine" << std::endl;
  co_return;
}

ReturnType<std::suspend_never > get_answer_coroutine() {
  std::cout << "Finding the answer..." << std::endl;
  co_await Awaitable<ReturnType<std::suspend_never>::promise_type>{42};
}

int main() {
  {
    // hello_coroutine
    pretty_print p{"case1"};
    auto_hello_coroutine();
  }
  {
    // hello_coroutine
    pretty_print p{"case2"};
    ReturnType c = manual_hello_coroutine();
    c.resume();
  }
  {
    pretty_print p{"case3"};
    auto a = get_answer_coroutine();
    int answer = a.get_the_answer();
    std::cout << "Answer is: " << answer << std::endl;
    a.resume();
  }
}
