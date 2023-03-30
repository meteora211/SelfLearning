#include <mutex>
#include <thread>
#include <iostream>
#include <string>
#include <chrono>

using namespace std::chrono_literals;

struct print_helper {
  print_helper() {
    std::cout << "==============================" << std::endl;
  }
  ~print_helper() {
    std::cout << "==============================" << std::endl;
  }
};

class mutex_sim {
public:
  mutex_sim() = default;
  explicit mutex_sim (const std::string& name) : name_(name) {}
  ~mutex_sim() = default;
  void lock() {
    std::cout << "locking: " << name_ << std::endl;
  }
  void unlock() {
    std::cout << "unlocking: " << name_ << std::endl;
  }
  bool try_lock() {
    std::cout << "try locking: " << name_ << std::endl;
    return true;
  }

private:
  std::string name_;
};

class mutex_real {
public:
  mutex_real() = default;
  ~mutex_real() = default;
  explicit mutex_real (const std::string& name) : name_(name) {}

  std::mutex& get_mutex() {
    return mutex_;
  }

private:
  std::mutex mutex_;
  std::string name_;
};

void deadlock_test(mutex_real& a, mutex_real& b) {
  std::cout << "deadlock test begin" << std::endl;
  std::lock_guard la(a.get_mutex());
  std::this_thread::sleep_for(1000ms);
  std::lock_guard lb(b.get_mutex());
  std::cout << "deadlock test end" << std::endl;
}

void lock_test(mutex_real& a, mutex_real& b) {
  std::cout << "lock test begin" << std::endl;
  std::lock(a.get_mutex(), b.get_mutex());
  // XXX: lock_guard won't lock mutex again with adopt_lock option.
  std::lock_guard la(a.get_mutex(), std::adopt_lock);
  std::this_thread::sleep_for(1000ms);
  std::lock_guard lb(b.get_mutex(), std::adopt_lock);
  std::cout << "lock test end" << std::endl;
}

int main() {
  mutex_sim sim_a{"a"}, sim_b{"b"};
  {
    print_helper p;
    std::cout << "lock guard simulation" << std::endl;
    std::lock_guard lg(sim_a);
    // XXX: lock_gurad has no unlock/lock interface
    // lg.unlock();
    // lg.lock();
  }
  {
    print_helper p;
    std::cout << "unique_lock simulation" << std::endl;
    std::unique_lock ul(sim_a);
    ul.unlock();
    ul.lock();
  }
  {
    print_helper p;
    std::cout << "lock simulation" << std::endl;
    std::lock(sim_a, sim_b);
  }
  {
    print_helper p;
    std::cout << "adapt lock simulation" << std::endl;
    std::lock(sim_a, sim_b);
    // XXX: lock_guard won't lock mutex again with adopt_lock option.
    // And lock_guard only accept adopt_lock. defer_lock/try_to_lock is not supported
    std::lock_guard la(sim_a, std::adopt_lock);
    std::lock_guard lb(sim_b, std::adopt_lock);
  }
  {
    print_helper p;
    std::cout << "defer lock simulation" << std::endl;
    // XXX: unique_lock won't lock mutex with defer_lock option.
    std::unique_lock la(sim_a, std::defer_lock);
    std::unique_lock lb(sim_b, std::defer_lock);
    // XXX: lock la and lb, not sim_a and sim_b.
    std::lock(la, lb);
  }
  {
    print_helper p;
    std::cout << "scoped lock simulation" << std::endl;
    std::scoped_lock sl(sim_a, sim_b);
  }

  mutex_real real_a{"a"}, real_b{"b"};
  // std::thread ta(deadlock_test, std::ref(real_a), std::ref(real_b));
  // std::thread tb(deadlock_test, std::ref(real_b), std::ref(real_a));
  std::thread ta(lock_test, std::ref(real_a), std::ref(real_b));
  std::thread tb(lock_test, std::ref(real_b), std::ref(real_a));
  ta.join();
  tb.join();
}
