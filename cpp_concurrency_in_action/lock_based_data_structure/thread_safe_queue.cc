#include <mutex>
#include <iostream>
#include <queue>
#include <condition_variable>
#include <thread>

template<typename T>
class ThreadSafeQueue {
public:
  ThreadSafeQueue() = default;
  ThreadSafeQueue(const ThreadSafeQueue& rhs) {
    std::unique_lock l(rhs.m_);
    data_ = rhs.data_;
  }
  explicit ThreadSafeQueue(const std::queue<T>& data) : data_(data) {}

  ~ThreadSafeQueue() = default;

  void push(T elem) {
    {
      std::unique_lock l(m_);
      data_.push(elem);
      // XXX: notify_one within a lock
      // It's okay and no error, but might have a little bit perf drawback
      // compare to outside a lock
      // cv_.notify_one();
    }
    cv_.notify_one();
  }

  void wait_pop(T& value) {
    std::unique_lock l(m_);
    cv_.wait(l, [this]{return !data_.empty();});
    value = std::move(data_.front());
    data_.pop();
  }

  bool try_pop(T& value) {
    std::unique_lock l(m_);
    if (data_.empty()) return false;
    value = std::move(data_.front());
    data_.pop();
    return true;
  }

  bool empty() const {
    std::unique_lock l(m_);
    return data_.empty();
  }

  void clear_and_print() {
    std::unique_lock l(m_);
    while (!data_.empty()) {
      auto&& v = data_.top();
      std::cout << v << " ";
      data_.pop();
    }
    std::cout << std::endl;
  }

  size_t size() const {
    std::unique_lock l(m_);
    return data_.size();
  }

private:
  std::queue<T> data_;
  mutable std::mutex m_;
  std::condition_variable cv_;
};

template<typename T>
class ThreadSafeLinkedList {
public:
  struct node {
    std::unique_ptr<node> next;
    T data_;
    node(T data) : data_(data) {}
  };

  ThreadSafeLinkedList() : tail_(nullptr) {};

  ~ThreadSafeLinkedList() = default;

  void push(T elem) {
    {
      std::unique_lock l(m_);
      auto p = std::make_unique<node>(elem);
      auto* tmp = p.get();
      if (tail_) {
        tail_->next = std::move(p);
      } else {
        head_ = std::move(p);
      }
      tail_ = tmp;
      cv_.notify_one();
    }
  }

  void wait_pop(T& value) {
    std::unique_lock l(m_);
    cv_.wait(l, [&]{return head_ != nullptr;});
    value = std::move(head_->data_);
    head_ = std::move(head_->next);
    // XXX: set tail_ back is important!
    if (head_ == nullptr) {
      tail_ = nullptr;
    }
  }

  bool try_pop(T& value) {
    std::unique_lock l(m_);
    if (head_ == nullptr) return false;
    value = std::move(head_->data_);
    head_ = std::move(head_->next);
    // XXX: set tail_ back is important!
    if (head_ == nullptr) {
      tail_ = nullptr;
    }
    return true;
  }

  bool empty() const {
    std::unique_lock l(m_);
    return head_ == nullptr;
  }

  void clear_and_print() {
    T value;
    while (try_pop(value)) {
      std::cout << value << " ";
    }
    std::cout << std::endl;
  }

  size_t size() const {
    std::unique_lock l(m_);
    size_t size = 0;
    auto tmp = head_.get();
    while (tmp != tail_) {
      ++size;
      tmp = tmp->next.get();
    }
    return size;
  }

private:
  std::unique_ptr<node> head_;
  node* tail_;
  mutable std::mutex m_;
  std::condition_variable cv_;
};

template<typename T>
class ThreadSafeQueueFineGrained {
public:
  struct node {
    std::unique_ptr<node> next;
    std::shared_ptr<T> data_;
  };
  ThreadSafeQueueFineGrained() : head_(std::make_unique<node>()), tail_(head_.get()) {};
  ~ThreadSafeQueueFineGrained() = default;

  void push(T elem) {
    {
      auto p = std::make_unique<node>();
      auto* tmp_ptr = p.get();
      auto tmp_data = std::make_shared<T>(std::move(elem));
      {
        std::unique_lock l(tail_mutex_);
        tail_->data_ = tmp_data;
        tail_->next = std::move(p);
        tail_ = tmp_ptr;
      }
      cv_.notify_one();
    }
  }

  std::shared_ptr<T> wait_pop () {
    std::unique_lock l(head_mutex_);
    cv_.wait(l, [&]{return head_.get() != get_tail();});
    auto value = head_->data_;
    head_ = std::move(head_->next);
    return value;
  }

  bool try_pop(T& value) {
    std::unique_lock l(head_mutex_);
    if (head_.get() == get_tail()) return false;
    value = *(head_->data_);
    head_ = std::move(head_->next);
    return true;
  }

  node* get_tail() const {
    std::unique_lock l(tail_mutex_);
    return tail_;
  }

  bool empty() const {
    std::unique_lock l(head_mutex_);
    return head_.get() == get_tail();
  }

  void clear_and_print() {
    T value;
    while (try_pop(value)) {
      std::cout << value << " ";
    }
    std::cout << std::endl;
  }

  size_t size() const {
    std::unique_lock l(head_mutex_);
    size_t size = 0;
    auto tmp = head_.get();
    while (tmp != get_tail()) {
      ++size;
      tmp = tmp->next.get();
    }
    return size;
  }

private:
  std::unique_ptr<node> head_;
  node* tail_;
  mutable std::mutex head_mutex_;
  mutable std::mutex tail_mutex_;
  std::condition_variable cv_;

};

int main() {
  /* ThreadSafeQueue<int> queue; */
  /* auto producer = [&](){ */
  /*   for (int i = 0; i < 10; ++i) { */
  /*     queue.push(i); */
  /*     std::cout << "size: " << queue.size() << " push: " << i << std::endl; */
  /*   } */
  /* }; */

  /* auto consumer = [&](){ */
  /*   for (int i = 0; i < 10; ++i) { */
  /*     int value; */
  /*     queue.wait_pop(value); */
  /*     std::cout << "size: " << queue.size() << " pop: " << value << std::endl; */
  /*   } */
  /* }; */

  /* std::thread t1(producer); */
  /* std::thread t2(consumer); */

  /* t1.join(); */
  /* t2.join(); */

  /* ThreadSafeLinkedList<int> queue_ll; */
  /* auto producer_ll = [&](){ */
  /*   for (int i = 0; i < 10; ++i) { */
  /*     queue_ll.push(i); */
  /*     std::cout << "size: " << queue_ll.size() << " push: " << i << std::endl; */
  /*   } */
  /* }; */

  /* auto consumer_ll = [&](){ */
  /*   for (int i = 0; i < 10; ++i) { */
  /*     int value; */
  /*     queue_ll.wait_pop(value); */
  /*     std::cout << "size: " << queue_ll.size() << " pop: " << value << std::endl; */
  /*   } */
  /* }; */
  /* std::thread t3(producer_ll); */
  /* std::thread t4(consumer_ll); */

  /* t3.join(); */
  /* t4.join(); */

  ThreadSafeQueueFineGrained<int> queue_fg;
  auto producer_fg = [&](){
    for (int i = 0; i < 10; ++i) {
      queue_fg.push(i);
      std::cout << "size: " << queue_fg.size() << " push: " << i << std::endl;
    }
  };

  auto consumer_fg = [&](){
    for (int i = 0; i < 10; ++i) {
      auto value = queue_fg.wait_pop();
      std::cout << "size: " << queue_fg.size() << " pop: " << *value << std::endl;
    }
  };
  std::thread t5(producer_fg);
  std::thread t6(consumer_fg);

  t5.join();
  t6.join();
}