///////////////////////////////////////////////////////////////////////////
// Example source code for blog post:
// "C++ Coroutines: Understanding Symmetric-Transfer"
//
// Implementation of a naive 'task' coroutine type.

////////////////////////////////////////////////////////////////////////////////////////
//                                   WTF WARNING                                      //
////////////////////////////////////////////////////////////////////////////////////////

#include <coroutine>
#include <iostream>
#include <utility>

using namespace std;

class task {
public:
  class promise_type {
  public:
    task get_return_object() noexcept {
	  std::cout << "[task::promise] get_return_object: " << this << std::endl;
      return task{coroutine_handle<promise_type>::from_promise(*this)};
    }

    suspend_always initial_suspend() noexcept {
	  std::cout << "[task::promise] initial_suspend: " << this << std::endl;
      return {};
    }

    /* void return_void() noexcept {} */
    int return_value(int value=0) noexcept {return value;}

    void unhandled_exception() noexcept {
      std::terminate();
    }

    struct final_awaiter {
      bool await_ready() noexcept {
        return false;
      }
      void await_suspend(coroutine_handle<promise_type> h) noexcept {
		std::cout << "[task::final_awaiter] await_suspend and resume: " << this << std::endl;
        h.promise().continuation.resume();
      }
      void await_resume() noexcept {}
    };

    final_awaiter final_suspend() noexcept {
	  std::cout << "[task::promise] final_suspend: " << this << std::endl;
      return {};
    }

    coroutine_handle<> continuation;
  };

  task(task&& t) noexcept
  : coro_(std::exchange(t.coro_, {}))
  {
	  std::cout << "[task] ctor: " << this << std::endl;
  }

  ~task() {
	std::cout << "[task] dtor: " << this << std::endl;
    if (coro_)
      coro_.destroy();
  }

  class awaiter {
  public:
    bool await_ready() noexcept {
      return false;
    }

    void await_suspend(coroutine_handle<> continuation) noexcept {
      // Store the continuation in the task's promise so that the final_suspend()
      // knows to resume this coroutine when the task completes.
      coro_.promise().continuation = continuation;

      // Then we resume the task's coroutine, which is currently suspended
      // at the initial-suspend-point (ie. at the open curly brace).
	  std::cout << "[task::awaiter] await_suspend and resume: " << this << std::endl;
      coro_.resume();
    }

    void await_resume() noexcept {}
  private:
    friend task;
    explicit awaiter(coroutine_handle<promise_type> h) noexcept
    : coro_(h)
    {}

    coroutine_handle<promise_type> coro_;
  };

  awaiter operator co_await() && noexcept {
	std::cout << "[task] co_await: " << this << std::endl;
    return awaiter{coro_};
  }

private:
  explicit task(coroutine_handle<promise_type> h) noexcept
  : coro_(h)
  {}

  coroutine_handle<promise_type> coro_;
};


struct sync_wait_task {
    struct promise_type {
        sync_wait_task get_return_object() noexcept {
            return sync_wait_task{coroutine_handle<promise_type>::from_promise(*this)};
        }

        suspend_never initial_suspend() noexcept { return{}; }

        suspend_always final_suspend() noexcept { return{}; }

        void return_void() noexcept {}

        void unhandled_exception() noexcept { std::terminate(); }
    };

    coroutine_handle<promise_type> coro_;

    explicit sync_wait_task(coroutine_handle<promise_type> h) noexcept : coro_(h) {}

    sync_wait_task(sync_wait_task&& t) noexcept : coro_(t.coro_) {
        t.coro_ = {};
    }

    ~sync_wait_task() {
        if (coro_) {
            coro_.destroy();
        }
    }

    static sync_wait_task start(task&& t) {
		std::cout << "[sync_wait_task] START: " << &t << std::endl;
        co_await std::move(t);
		std::cout << "[sync_wait_task] RETURN" << std::endl;
    }

    bool done() {
        return coro_.done();
    }
};

struct manual_executor {
    struct schedule_op {
        manual_executor& executor_;
        schedule_op* next_ = nullptr;
        coroutine_handle<> continuation_;

        schedule_op(manual_executor& executor)
        : executor_(executor)
        {}

        bool await_ready() noexcept { return false; }

        void await_suspend(coroutine_handle<> continuation) noexcept {
			std::cout << "[schedule_op] await_suspend: " << this << std::endl;
            continuation_ = continuation;
            next_ = executor_.head_;
            executor_.head_ = this;
        }

        void await_resume() noexcept {}
    };

    schedule_op* head_ = nullptr;

    schedule_op schedule() noexcept {
		std::cout << "[schedule_op] schedule: " << this << std::endl;
        return schedule_op{*this};
    }

    void drain() {
        while (head_ != nullptr) {
            auto* item = head_;
            head_ = item->next_;
            item->continuation_.resume();
        }
    }

    void sync_wait(task&& t) {
        auto t2 = sync_wait_task::start(std::move(t));
        while (!t2.done()) {
            drain();
        }
    }
};

/* task foo(manual_executor& ex) { */
/*     std::cout << "inside foo()\n"; */
/*     co_await ex.schedule(); */
/*     std::cout << "about to return from foo()\n"; */
/*     co_return; */
/* } */

/* task bar(manual_executor& ex) { */
/*     std::cout << "about to call foo()\n"; */
/*     co_await foo(ex); */
/*     std::cout << "done calling foo()\n"; */
/* } */

task completes_synchronously(int i) {
    // co_await task::promise_type::initial_suspend();
	// --> co_await suspend_always;
    std::cout << "completes: " << i << std::endl;
    co_return i;
    // co_await task::promise_type::final_suspend();
	// --> co_await final_awaiter;
}


task loop_synchronously(int count) {
    std::cout << "loop_synchronously(" << count << ")" << std::endl;
    for (int i = 0; i < count; ++i) {
		std::cout << "loop_synchronously loop (" << i << ")" << std::endl;
        co_await completes_synchronously(i);
		// auto task_inner_i = completes_synchronously(i);
		// co_await task_inner_i;
		// --> awaiter = task_inner_i::operator co_await();
		// --> awaiter = task_inner_i::awaiter;
		// --> awaiter.await_ready() ? awaiter.await_resume() : awaiter().await_suspend();
    }
    std::cout << "loop_synchronously(" << count << ") returning" << std::endl;
}

int main() {
    manual_executor ex;
    /* ex.sync_wait(bar(ex)); */
    ex.sync_wait(loop_synchronously(5));
    /* ex.sync_wait(loop_synchronously(100)); */
    /* ex.sync_wait(loop_synchronously(1000)); */
    /* ex.sync_wait(loop_synchronously(100'000)); */
    /* ex.sync_wait(loop_synchronously(1'000'000)); // Crashes with stack-overflow! */
    return 0;
}
