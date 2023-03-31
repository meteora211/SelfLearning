#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <shared_mutex>

template<typename T>
class QueueTest {
public:
  QueueTest() = default;
  ~QueueTest() = default;

  void push(const T& data) {
    std::unique_lock ul(mutex_);
    data_.push(data);
    cv_.notify_one();
  }

  T pop() {
    std::unique_lock ul(mutex_);
    cv_.wait(ul, [&]{return !data_.empty();});
    T tmp = data_.front();
    data_.pop();
    return tmp;
  }

  T front() const {
    std::shared_lock sl(mutex_);
    return data_.pop();
  }

  bool empty() const {
    std::shared_lock sl(mutex_);
    return data_.empty();
  }

  size_t size() const {
    std::shared_lock sl(mutex_);
    return data_.size();

  }


private:
  std::queue<T> data_;
  mutable std::shared_mutex mutex_;
  std::condition_variable_any cv_;
};

int main() {
  QueueTest<int> queue;

  auto producer = [&](){
    for (int i = 0; i < 10; ++i) {
      std::cout << "size: " << queue.size() << " push: " << i << std::endl;
      queue.push(i);
    }
  };

  auto consumer = [&](){
    for (int i = 0; i < 10; ++i) {
      std::cout << "size: " << queue.size() << " pop: " << queue.pop() << std::endl;
    }
  };

  std::thread t1(producer);
  std::thread t2(consumer);

  t1.join();
  t2.join();

  std::cout << "Done with queue size: " << queue.size() << std::endl;

}
