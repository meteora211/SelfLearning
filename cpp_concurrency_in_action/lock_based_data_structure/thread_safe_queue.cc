#include <mutex>
#include <iostream>
#include <queue>
#include <condition_variable>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

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

template <typename T>
class ConcurrentQueue {
 public:
  ConcurrentQueue() : head_(new Node), tail_(head_.get()) {}

  ConcurrentQueue(const ConcurrentQueue&) = delete;

  ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

  void push(T x) {
    auto new_val = std::make_shared<T>(std::move(x));
    auto new_node = std::make_unique<Node>();
    Node* new_tail_node = new_node.get();
    {
      std::lock_guard<std::mutex> l(tail_mutex_);
      tail_->v = new_val;
      tail_->next = std::move(new_node);
      tail_ = new_tail_node;
    }
    cv_.notify_one();
  }

  std::shared_ptr<T> try_pop() {
    std::unique_ptr<Node> head_node = try_pop_head();
    return head_node ? head_node->v : nullptr;
  }

  bool try_pop(T& res) {
    std::unique_ptr<Node> head_node = try_pop_head(res);
    return head_node != nullptr;
  }

  std::shared_ptr<T> wait_and_pop() {
    std::unique_ptr<Node> head_node = wait_pop_head();
    return head_node->v;
  }

  void wait_and_pop(T& res) { wait_pop_head(res); }

  bool empty() const {
    std::lock_guard<std::mutex> l(head_mutex_);
    return head_.get() == get_tail();
  }

 private:
  struct Node {
    std::shared_ptr<T> v;
    std::unique_ptr<Node> next;
  };

 private:
  std::unique_ptr<Node> try_pop_head() {
    std::lock_guard<std::mutex> l(head_mutex_);
    if (head_.get() == get_tail()) {
      return nullptr;
    }
    return pop_head();
  }

  std::unique_ptr<Node> try_pop_head(T& res) {
    std::lock_guard<std::mutex> l(head_mutex_);
    if (head_.get() == get_tail()) {
      return nullptr;
    }
    res = std::move(*head_->v);
    return pop_head();
  }

  std::unique_ptr<Node> wait_pop_head() {
    std::unique_lock<std::mutex> l(wait_for_data());
    return pop_head();
  }

  std::unique_ptr<Node> wait_pop_head(T& res) {
    std::unique_lock<std::mutex> l(wait_for_data());
    res = std::move(*head_->v);
    return pop_head();
  }

  std::unique_lock<std::mutex> wait_for_data() {
    std::unique_lock<std::mutex> l(head_mutex_);
    cv_.wait(l, [this] { return head_.get() != get_tail(); });
    return l;
  }

  std::unique_ptr<Node> pop_head() {
    std::unique_ptr<Node> head_node = std::move(head_);
    head_ = std::move(head_node->next);
    return head_node;
  }

  Node* get_tail() {
    std::lock_guard<std::mutex> l(tail_mutex_);
    return tail_;
  }

 private:
  std::unique_ptr<Node> head_;
  Node* tail_ = nullptr;
  std::mutex head_mutex_;
  mutable std::mutex tail_mutex_;
  std::condition_variable cv_;
};

int main() {
  const int NUM = 65500;
  {
    ThreadSafeQueue<int> queue;
    auto producer = [&](){
      for (int i = 0; i < NUM; ++i) {
        queue.push(i);
      }
    };

    auto consumer = [&](){
      for (int i = 0; i < NUM; ++i) {
        int value;
        queue.wait_pop(value);
      }
    };

    auto start = std::chrono::high_resolution_clock::now();
    std::thread t1(producer);
    std::thread t2(consumer);

    t1.join();
    t2.join();
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << (end - start).count()  / 1000 << std::endl;
  }

  {
    ThreadSafeLinkedList<int> queue_ll;
    auto producer_ll = [&](){
      for (int i = 0; i < NUM; ++i) {
        queue_ll.push(i);
      }
    };

    auto consumer_ll = [&](){
      for (int i = 0; i < NUM; ++i) {
        int value;
        queue_ll.wait_pop(value);
      }
    };

    auto start = std::chrono::high_resolution_clock::now();
    std::thread t3(producer_ll);
    std::thread t4(consumer_ll);

    t3.join();
    t4.join();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << (end - start).count()  / 1000 << std::endl;
  }

  {
    ThreadSafeQueueFineGrained<int> queue_fg;
    auto producer_fg = [&](){
      for (int i = 0; i < NUM; ++i) {
        queue_fg.push(i);
      }
    };

    auto consumer_fg = [&](){
      for (int i = 0; i < NUM; ++i) {
        auto value = queue_fg.wait_pop();
      }
    };

    auto start = std::chrono::high_resolution_clock::now();
    std::thread t5(producer_fg);
    std::thread t6(consumer_fg);

    t5.join();
    t6.join();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << (end - start).count()  / 1000 << std::endl;
  }

  {
    ConcurrentQueue<int> c_queue;
    auto producer_fg = [&](){
      for (int i = 0; i < NUM; ++i) {
        c_queue.push(i);
      }
    };

    auto consumer_fg = [&](){
      for (int i = 0; i < NUM; ++i) {
        auto value = c_queue.wait_and_pop();
      }
    };

    auto start = std::chrono::high_resolution_clock::now();
    std::thread t7(producer_fg);
    std::thread t8(consumer_fg);

    t7.join();
    t8.join();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << (end - start).count()  / 1000 << std::endl;

  }

}
