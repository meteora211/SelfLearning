#include <concepts>
#include <coroutine>
#include <iostream>
#include <string>
#include <atomic>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

// XXX: I don't really understant the example.
class async_manual_reset_event
{
public:

  async_manual_reset_event(bool initiallySet = false) noexcept;

  // No copying/moving
  async_manual_reset_event(const async_manual_reset_event&) = delete;
  async_manual_reset_event(async_manual_reset_event&&) = delete;
  async_manual_reset_event& operator=(const async_manual_reset_event&) = delete;
  async_manual_reset_event& operator=(async_manual_reset_event&&) = delete;

  bool is_set() const noexcept;

  struct awaiter;
  awaiter operator co_await() const noexcept;

  void set() noexcept;
  void reset() noexcept;

private:

  friend struct awaiter;

  // - 'this' => set state
  // - otherwise => not set, head of linked list of awaiter*.
  mutable std::atomic<void*> m_state;

};

struct async_manual_reset_event::awaiter
{
  awaiter(const async_manual_reset_event& event) noexcept
  : m_event(event)
  {}

  bool await_ready() const noexcept;
  bool await_suspend(std::coroutine_handle<> awaitingCoroutine) noexcept;
  void await_resume() noexcept {}

private:
  friend struct async_manual_reset_event;

  const async_manual_reset_event& m_event;
  std::coroutine_handle<> m_awaitingCoroutine;
  awaiter* m_next;
};

bool async_manual_reset_event::awaiter::await_ready() const noexcept
{
  return m_event.is_set();
}

bool async_manual_reset_event::awaiter::await_suspend(
  std::coroutine_handle<> awaitingCoroutine) noexcept
{
  // Special m_state value that indicates the event is in the 'set' state.
  const void* const setState = &m_event;

  // Stash the handle of the awaiting coroutine.
  m_awaitingCoroutine = awaitingCoroutine;

  // Try to atomically push this awaiter onto the front of the list.
  void* oldValue = m_event.m_state.load(std::memory_order_acquire);
  do
  {
    // Resume immediately if already in 'set' state.
    if (oldValue == setState) return false;

    ////////////////////////////////////////////////////////////////////////////////////////
    //                                   WTF WARNING                                      //
    ////////////////////////////////////////////////////////////////////////////////////////
    // XXX: Why an oldValue, which is atomic<void*>, can be casted to awaiter*?
    // The anwser: When await tasks, line no. 97 is first reached and pushes OldValue.
    // If the OldValue is pointer to async_manual_reset_event, then line 79 will return true.
    // Else, m_state exchanges the pointer `this`, which is a pointer to awaiter. And thus,
    // m_next can be retieved by casting oldValue to awaiter*.
    // if (oldValue is pointer to m_event) return false;
    // m_next = cast oldValue to awaiter* // the very first will be nullptr.
    // while ( m_state == oldValue ? m_state = this : oldValue = m_state  )
    // Update linked list to point at current head.
    m_next = static_cast<awaiter*>(oldValue);

    // Finally, try to swap the old list head, inserting this awaiter
    // as the new list head.
  } while (!m_event.m_state.compare_exchange_weak(
             oldValue,
             this,
             std::memory_order_release,
             std::memory_order_acquire));

  // Successfully enqueued. Remain suspended.
  return true;
}

async_manual_reset_event::async_manual_reset_event(
  bool initiallySet) noexcept
: m_state(initiallySet ? this : nullptr)
{}

bool async_manual_reset_event::is_set() const noexcept
{
  return m_state.load(std::memory_order_acquire) == this;
}

void async_manual_reset_event::reset() noexcept
{
  void* oldValue = this;
  m_state.compare_exchange_strong(oldValue, nullptr, std::memory_order_acquire);
}

void async_manual_reset_event::set() noexcept
{
  ////////////////////////////////////////////////////////////////////////////////////////
  //                                   WTF WARNING                                      //
  ////////////////////////////////////////////////////////////////////////////////////////
  // XXX: Here m_state is exchanged back to pointer to async_manual_reset_event
  // and then check if oldValue is already point to async_manual_reset_event,
  // if not, then oldValue is awaiter* from `await_suspend`, and resume waiters.
  // else just return. WTF!!!!
  // Needs to be 'release' so that subsequent 'co_await' has
  // visibility of our prior writes.
  // Needs to be 'acquire' so that we have visibility of prior
  // writes by awaiting coroutines.
  void* oldValue = m_state.exchange(this, std::memory_order_acq_rel);
  if (oldValue != this)
  {
    // Wasn't already in 'set' state.
    // Treat old value as head of a linked-list of waiters
    // which we have now acquired and need to resume.
    auto* waiters = static_cast<awaiter*>(oldValue);
    while (waiters != nullptr)
    {
      // Read m_next before resuming the coroutine as resuming
      // the coroutine will likely destroy the awaiter object.
      auto* next = waiters->m_next;
      waiters->m_awaitingCoroutine.resume();
      waiters = next;
    }
  }
}

async_manual_reset_event::awaiter
async_manual_reset_event::operator co_await() const noexcept
{
  return awaiter{ *this };
}

// A simple task-class for void-returning coroutines.
struct task
{
    struct promise_type
    {
        task get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
};

/* task example(async_manual_reset_event& event) */
/* { */
/*     co_await event; */
/* } */

int long_computation() {
  std::cout << "start long long time-consuming computation" << std::endl;
  std::this_thread::sleep_for(1000ms);
  return 42;
}

int main() {
  int value = 0;
  async_manual_reset_event event;
  auto producer = [&](){
    value = long_computation();
    std::cout << "set event" << std::endl;
    event.set();
  };

  for (int i = 0; i < 10; ++i) {
    [&](int n) -> task {
      co_await event;
      std::cout << "computed value in " << n << ": " << value << std::endl;
    }(i);
  }

  auto do_other_stuff = [&](){
    std::cout << "do other stuff" << std::endl;
  };


  producer();
  do_other_stuff();
}
