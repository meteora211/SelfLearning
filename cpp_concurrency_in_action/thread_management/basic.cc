#include <iostream>
#include <thread>
#include <string>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <numeric>

void do_something() {
  std::cout << "do_something" << std::endl;
}

void do_something_else() {
  std::cout << "do_something_else" << std::endl;
}

void do_something_guard() {
  std::cout << "do_something_guard" << std::endl;
}

class BackgroundTask {
public:
  void operator()() {
    do_something();
    do_something_else();
  }
  void do_background_task (int i) {
    std::cout <<  i << " do_background_task" << std::endl;
  }
};

// RAII
class thread_guard {
public:
  explicit thread_guard(std::thread& t) : t_(t) {}
  thread_guard(const thread_guard&) = delete;
  thread_guard& operator=(const thread_guard&) = delete;
  ~thread_guard(){
    if (t_.joinable()) {
      t_.join();
    }
  }
  std::thread& t_;
};

class local_ref {
public:
  explicit local_ref(int& r) : ref(r) {}
  void operator()(){
    for (int i = 0; i < 100000; ++i) {
      ++ref;
    }
    std::cout << "final ref value: " << ref << std::endl;
  }
  int& ref;
};

void foo(int i, const std::string& s) {
  std::cout << "const version: " << i << s << std::endl;
}

void bar(int i, std::string& s) {
  std::cout << "nonconst version: " << i << s << std::endl;
}

void foo_move(std::unique_ptr<BackgroundTask> b) {
  b->do_background_task(3);
}

std::thread foo_create_thread() {
  return std::thread(do_something);
}

void foo_accept_thread(std::thread t) {
  t.join();
}

class scoped_thread {
public:
  explicit scoped_thread(std::thread& t) : t_(std::move(t)) {
    if (!t_.joinable()) {
      throw std::logic_error("no thread");
    }
  }
  scoped_thread(const scoped_thread&) = delete;
  scoped_thread& operator=(const scoped_thread&) = delete;
  ~scoped_thread(){
    std::cout << "scoped thread" << std::endl;
    t_.join();
  }
  std::thread t_;
};

class joining_thread {
public:
  explicit joining_thread(std::thread t) noexcept : t_(std::move(t)) {}

  template<typename Callable, typename... Args>
  joining_thread(Callable&& func, Args&&... args) :
    t_(std::forward<Callable>(func), std::forward<Args>(args)...) {}

  joining_thread(const joining_thread&) = delete;

  joining_thread& operator=(const joining_thread&) = delete;

  joining_thread(joining_thread&& rhs) noexcept {
    t_ = std::move(rhs.t_);
  }

  joining_thread& operator=(joining_thread&& rhs) noexcept {
    if (joinable()) {
      join();
    }
    t_ = std::move(rhs.t_);
    return *this;
  }

  ~joining_thread() {
    std::cout << "joining thread" << std::endl;
    if (t_.joinable()) {
      t_.join();
    }
  }

  bool joinable() const noexcept {
    return t_.joinable();
  }

  void join() {
    t_.join();
  }

  void detach() {
    t_.detach();
  }

  std::thread& as_thread() noexcept {
    return t_;
  }

  const std::thread& as_thread() const noexcept {
    return t_;
  }

  std::thread::id get_id() const noexcept {
    return t_.get_id();
  }

private:
  std::thread t_;
};

template<typename Iterator, typename T>
struct accumulate_block {
  void operator()(Iterator first, Iterator last, T& result) {
    result = std::accumulate(first, last, result);
    std::cout << "result: " << result << std::endl;
  }
};

template<typename Iterator, typename T>
T accumulate_parallel(Iterator first, Iterator last, T init) {
  const uint64_t length = std::distance(first, last);
  if (!length) return init;
  uint64_t max_threads = 25;
  uint64_t hardware_threads = std::thread::hardware_concurrency();

  uint64_t thread_num = hardware_threads ? hardware_threads : max_threads;

  uint64_t block_size = length / thread_num;

  std::vector<std::thread> thread_vec(thread_num);
  std::vector<T> result_vec(thread_num);

  accumulate_block<Iterator, T> accumulate_block_fn;
  auto block_start = first;
  auto block_end = first;
  for (uint64_t i = 0; i < thread_num; ++i) {
    std::advance(block_end, block_size);
    thread_vec[i] = std::thread(accumulate_block_fn, block_start, block_end, std::ref(result_vec[i]));
    std::advance(block_start, block_size);
  }
  std::for_each(thread_vec.begin(), thread_vec.end(), std::mem_fn(&std::thread::join));
  return std::accumulate(result_vec.begin(), result_vec.end(), init);
}

// basic 2.1
void thread_basic() {
  std::thread t_f(do_something);
  // XXX: Most_vexing_parse: T1 name(T2());
  // the C++ grammar cannot distinguish between
  // - the creation of an object parameter. name is an object of T1.
  // - specification of a function's type. name is a function with return type T1, and accept function pointer T1() as parameter
  // following code cause a compiler error.
  // std::thread t_c(BackgroundTask());
  std::thread t_c(BackgroundTask{});
  std::thread t_l([](){do_something();});
  t_f.join();
  t_c.join();
  t_l.join();

  int some_local_state = 0;
  std::thread t_1(local_ref{some_local_state});
  // XXX: cause a dangling pointer
  // t_1.detach();
  thread_guard g(t_1);
  do_something_else();
}

// param 2.2
void thread_param() {
  char buffer[1024];
  // XXX: following code cause undefined behavior.
  // Implicit conversion(char* -> const string&) may not complete
  // when oops is already finished.
  // std::thread t(foo, 3, buffer);
  std::thread t1(foo, 3, std::string(buffer));
  t1.detach();
  std::string data("test data");
  // XXX: need std::ref on non-const reference
  std::thread t2(bar, 3, std::ref(data));
  t2.detach();

  BackgroundTask b;
  std::thread t3(&BackgroundTask::do_background_task, &b, 3);
  t3.join();

  auto b_ptr = std::make_unique<BackgroundTask>();
  std::thread t4(foo_move, std::move(b_ptr));
  t4.join();
}

// ownership 2.3
void thread_ownership() {
  std::thread t1(do_something);
  std::thread t2 = std::move(t1);
  t1 = std::thread(do_something_else);
  std::thread t3;
  t3 = std::move(t2);
  /* t1 = std::move(t3); */
  t1.join();
  t3.join();

  foo_accept_thread(std::thread([]{do_something();}));
  std::thread t4(do_something);
  foo_accept_thread(std::move(t4));

  std::thread t5 = foo_create_thread();
  t5.join();

  {
    std::thread t6 = foo_create_thread();
    scoped_thread st(t6);
  }
}

void test_joining_thread() {
  std::thread t1(do_something);
  joining_thread jt(std::move(t1));
  jt.as_thread().join();
}

void test_accumulate_parallel() {
  std::vector<int> vec(100000);
  std::iota(vec.begin(), vec.end(), 0);
  auto res = accumulate_parallel(vec.begin(), vec.end(), int(0));
  auto ref = std::accumulate(vec.begin(), vec.end(), 0);
  std::cout << "res: " << res << " ref: " << ref << std::endl;
}

int main() {
  thread_basic();
  thread_param();
  thread_ownership();
  test_joining_thread();
  test_accumulate_parallel();
}
