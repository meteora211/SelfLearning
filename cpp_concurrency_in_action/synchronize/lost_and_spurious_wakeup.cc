#include <chrono>
#include <thread>
#include <future>
#include <iostream>
#include <condition_variable>

using namespace std::chrono_literals;

class LostWakeUp {
public:
  LostWakeUp() = default;
  ~LostWakeUp() = default;

  void waitForWork() {
    std::cout << std::this_thread::get_id() << " waiting..." << std::endl;
    std::unique_lock l(m_);
    cv_.wait(l, [&](){return ready_;});
    std::cout << std::this_thread::get_id() << " working..." << std::endl;
  }

  void waitForWorkWithoutPredicate() {
    std::unique_lock l(m_);
    std::cout << std::this_thread::get_id() << " waiting..." << std::endl;
    if (cv_.wait_for(l, 100ms) == std::cv_status::timeout) {
      std::cout << std::this_thread::get_id() << " time out!!!" << std::endl;
    } else {
      std::cout << std::this_thread::get_id() << " working without predicate..." << std::endl;
    }
  }

  void setReady() {
    std::unique_lock l(m_);
    ready_ = true;
    cv_.notify_one();
    std::cout << std::this_thread::get_id() << " ready!" << std::endl;
  }

  void setReadyWithLatency() {
    std::this_thread::sleep_for(20ms);
    setReady();
  }

  void run() {
    std::cout << "===================== Run start ======================" << std::endl;
    auto t1 = std::thread(&LostWakeUp::waitForWork, this);
    auto t2 = std::thread(&LostWakeUp::setReady, this);
    t1.join();
    t2.join();
    std::cout << "===================== Run End ======================" << std::endl;
  }

  void runWithoutPredicate() {
    std::cout << "===================== RunWithoutPredicate start ======================" << std::endl;
    auto t1 = std::thread(&LostWakeUp::waitForWorkWithoutPredicate, this);
    auto t2 = std::thread(&LostWakeUp::setReady, this);
    t1.join();
    t2.join();
    std::cout << "===================== RunWithoutPredicate end ======================" << std::endl;
  }

  void runWithLatency() {
    std::cout << "===================== RunWithLatency start ======================" << std::endl;
    auto t1 = std::thread(&LostWakeUp::waitForWorkWithoutPredicate, this);
    auto t2 = std::thread(&LostWakeUp::setReadyWithLatency, this);
    t1.join();
    t2.join();
    std::cout << "===================== RunWithLatency end ======================" << std::endl;
  }

private:
  std::condition_variable cv_;
  std::mutex m_;
  bool ready_ = false;
};

int main() {

  LostWakeUp l;
  // Normal case
  l.run();
  // With out Predicate, work thread may wait forever in following case:
  // - `setReady` thread obtain the mutex first
  // - `notify_one` is called and at the exactly same time point, work thread haven't reach the call `wait`
  // - thus signal sent by notify_one is missed, work thread keep waiting and waiting and ...
  l.runWithoutPredicate();
  // Give a latency to `setReady` thread to make sure `notify_one` is called AFTER `wait` in work thread.
  l.runWithLatency();

}
