Q: how to check if corotine_handle is done in C++
---
Raycast AI: In C++, you can check if a `coroutine_handle` is done by calling its `done()` member function. This function returns a `bool` value that indicates whether the coroutine has completed execution.

Here's an example:

```c++
#include <coroutine>

struct MyCoroutine {
    struct promise_type {
        auto get_return_object() { return MyCoroutine{handle_type::from_promise(*this)}; }
        auto initial_suspend() { return std::suspend_never{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }
        void return_void() {}
        void unhandled_exception() {}
    };

    using handle_type = std::coroutine_handle<promise_type>;

    MyCoroutine(handle_type handle) : handle_(handle) {}
    ~MyCoroutine() { handle_.destroy(); }

    bool done() const { return handle_.done(); }
    void resume() { handle_.resume(); }

private:
    handle_type handle_;
};

int main() {
    MyCoroutine coroutine = []() -> MyCoroutine {
        // Coroutine body
        co_return;
    }();

    while (!coroutine.done()) {
        coroutine.resume();
    }

    return 0;
}
```

In this example, the `MyCoroutine` class wraps a `coroutine_handle` and provides a `done()` member function to check if the coroutine has completed execution. The `main()` function creates an instance of `MyCoroutine` and repeatedly calls its `resume()` member function until the coroutine is done.
---
Q: why final_suspend returned suspend_always
---
Raycast AI: In C++, the `final_suspend()` function of a coroutine's promise type determines what happens when the coroutine is about to complete execution. It should return an awaitable object that specifies whether the coroutine should be suspended or not.

In the example I provided earlier, the `final_suspend()` function returns `std::suspend_always{}`. This means that the coroutine will always be suspended when it is about to complete execution, even if it was not suspended before. This is useful because it allows the caller of the coroutine to perform any necessary cleanup or post-processing after the coroutine has finished executing.

If `final_suspend()` returned `std::suspend_never{}`, the coroutine would not be suspended when it completes execution, and the caller would not have an opportunity to perform any final actions. This is generally not desirable, as it can lead to resource leaks or other issues.

So, in summary, `final_suspend()` returning `std::suspend_always{}` ensures that the coroutine is always suspended when it completes execution, giving the caller a chance to perform any necessary cleanup.
---
Q: if I change the return value of final_suspend to suspend_never, how to change caller code to make it work?
---
Raycast AI: If you change the return value of `final_suspend()` to `std::suspend_never{}`, the coroutine will not be suspended when it completes execution. This means that the caller code needs to be updated to handle this case appropriately.

Here's an updated example that shows how to modify the caller code to work with a coroutine that does not suspend on finalization:

```c++
#include <coroutine>
#include <iostream>

struct MyCoroutine {
    struct promise_type {
        auto get_return_object() { return MyCoroutine{handle_type::from_promise(*this)}; }
        auto initial_suspend() { return std::suspend_never{}; }
        auto final_suspend() noexcept { return std::suspend_never{}; }
        void return_void() {}
        void unhandled_exception() {}
    };

    using handle_type = std::coroutine_handle<promise_type>;

    MyCoroutine(handle_type handle) : handle_(handle) {}
    ~MyCoroutine() { handle_.destroy(); }

    bool done() const { return handle_.done(); }
    void resume() { handle_.resume(); }

private:
    handle_type handle_;
};

int main() {
    MyCoroutine coroutine = []() -> MyCoroutine {
        // Coroutine body
        std::cout << "Coroutine started" << std::endl;
        co_return;
    }();

    coroutine.resume();

    std::cout << "Coroutine finished" << std::endl;

    return 0;
}
```

In this example, the `final_suspend()` function of the `MyCoroutine` promise type returns `std::suspend_never{}`, indicating that the coroutine should not be suspended when it completes execution.

The `main()` function creates an instance of `MyCoroutine` and calls its `resume()` member function. This starts the coroutine and executes its body, which in this case simply prints a message to the console.

After the coroutine completes execution, the `main()` function prints another message to the console to indicate that the coroutine has finished.

Note that in this example, the `while` loop that was used to repeatedly call `resume()` in the previous example is no longer needed, since the coroutine is not suspended on finalization. Instead, the `resume()` function is called once to start the coroutine, and the program continues executing normally after the coroutine has finished.
---
Q: Did that mean if I use `done()` method in coroutine_handle within a loop, the final_suspend should return suspend_always
---
Raycast AI: Yes, that's correct. If you use the `done()` method of a `coroutine_handle` to check if a coroutine has completed execution within a loop, then the `final_suspend()` function of the coroutine's promise type should return `std::suspend_always{}`.

Here's an example to illustrate this:

```c++
#include <coroutine>
#include <iostream>

struct MyCoroutine {
    struct promise_type {
        auto get_return_object() { return MyCoroutine{handle_type::from_promise(*this)}; }
        auto initial_suspend() { return std::suspend_never{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }
        void return_void() {}
        void unhandled_exception() {}
    };

    using handle_type = std::coroutine_handle<promise_type>;

    MyCoroutine(handle_type handle) : handle_(handle) {}
    ~MyCoroutine() { handle_.destroy(); }

    bool done() const { return handle_.done(); }
    void resume() { handle_.resume(); }

private:
    handle_type handle_;
};

int main() {
    MyCoroutine coroutine = []() -> MyCoroutine {
        // Coroutine body
        for (int i = 0; i < 5; ++i) {
            std::cout << "Coroutine iteration " << i << std::endl;
            co_await std::suspend_always{};
        }
        std::cout << "Coroutine finished" << std::endl;
        co_return;
    }();

    while (!coroutine.done()) {
        coroutine.resume();
    }

    return 0;
}
```

In this example, the `MyCoroutine` class is similar to the previous example, but the `final_suspend()` function of the promise type now returns `std::suspend_always{}` instead of `std::suspend_never{}`.

The `main()` function creates an instance of `MyCoroutine` and repeatedly calls its `resume()` member function within a loop until the coroutine is done. This loop checks if the coroutine is done by calling its `done()` member function, which in turn calls the `done()` method of the `coroutine_handle`.

The coroutine body contains a loop that iterates five times, printing a message to the console on each iteration and suspending the coroutine using `co_await std::suspend_always{}`. After the loop completes, the coroutine prints another message to the console to indicate that it has finished.

By returning `std::suspend_always{}` from `final_suspend()`, the coroutine is guaranteed to be suspended when it completes execution, which allows the caller code to perform any necessary cleanup or post-processing before the coroutine is destroyed.
