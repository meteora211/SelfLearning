#include <concepts>
#include <coroutine>
#include <iostream>
#include <string>
struct pretty_print {
  pretty_print(const std::string& name) : name_(name) {
    std::cout << "==================" << name_ << " [start]====================" << std::endl;
  }
  ~pretty_print() {
    std::cout << "===================" << name_ << " [end]=====================" << std::endl;
  }
  std::string name_;
};

template<typename T = std::suspend_never>
struct ReturnType {
  struct promise_type {
    promise_type() {
      std::cout << "[promise_type] Constructing promise" << std::endl;
    }
    ~promise_type() {
      std::cout << "[promise_type] Deconstructing promise" << std::endl;
    }
    void* operator new(std::size_t size) {
      std::cout << "[promise_type] Operator new" << std::endl;
      return std::malloc(size);
    }
    void operator delete(void* ptr) {
      std::cout << "[promise_type] Operator delete" << std::endl;
      std::free(ptr);
    }
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
    std::cout << "Got it!" << std::endl;
    return h_.promise().answer;
  }
  void provide_the_answer(int answer) {
    std::cout << "Providing it!" << std::endl;
    h_.promise().answer = answer;
    h_.resume();
  }
};

template<typename Promise>
struct FindingAwaitable {
  FindingAwaitable (int answer) : answer_(answer) {}
  bool await_ready() const noexcept {
    std::cout << "[FindingAwaitable] In await_ready function\n";
    return false;
  }
  void await_suspend(std::coroutine_handle<Promise> h) {
    std::cout << "[FindingAwaitable] Found the answer" << std::endl;
    h.promise().answer = answer_;
  }
  void await_resume() const noexcept {
    std::cout << "[FindingAwaitable] In await_resume function\n";
  }
  int answer_;
};

template<typename Promise>
struct WaitingAwaitable {
  WaitingAwaitable () = default;
  bool await_ready() const noexcept {
    std::cout << "[WaitingAwaitable] In await_ready function\n";
    return false;
  }
  void await_suspend(std::coroutine_handle<Promise> h) {
    std::cout << "[WaitingAwaitable] Waiting the answer" << std::endl;
    h_  = h;
  }
  int await_resume() const noexcept {
    std::cout << "[WaitingAwaitable] In await_resume function\n";
    return h_.promise().answer;
  }
  std::coroutine_handle<Promise> h_;
};

ReturnType<std::suspend_never > auto_hello_coroutine() {
  std::cout << "auto hello coroutine" << std::endl;
  co_return;
}

ReturnType<std::suspend_always > manual_hello_coroutine() {
  std::cout << "manual hello coroutine" << std::endl;
  co_return;
}

ReturnType<std::suspend_never > finding_answer_coroutine() {
  std::cout << "Finding the answer..." << std::endl;
  co_await FindingAwaitable<ReturnType<std::suspend_never>::promise_type>{42};
}

ReturnType<std::suspend_never > waiting_answer_coroutine() {
  std::cout << "Waiting the answer..." << std::endl;
  auto answer = co_await WaitingAwaitable<ReturnType<std::suspend_never>::promise_type>{};
  std::cout << "Got it: " << answer << std::endl;
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
    pretty_print p{"get data out of coroutine"};
    auto a = finding_answer_coroutine();
    int answer = a.get_the_answer();
    std::cout << "Answer is: " << answer << std::endl;
    a.resume();
  }
  {
    pretty_print p{"get data into coroutine"};
    auto a = waiting_answer_coroutine();
    a.provide_the_answer(42);
  }
}
