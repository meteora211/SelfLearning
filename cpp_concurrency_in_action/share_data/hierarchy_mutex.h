#pragma once
#include <mutex>
#include <thread>
#include <iostream>
#include <string>
#include <chrono>
#include <limits>


class hierarchy_mutex {
public:
  explicit hierarchy_mutex(int level) : cur_hierarchy_(level), prev_hierarchy_(0) {}

  void lock () {
    validate_hierarchy();
    m_.lock();
    update_hierarchy();
  }

  void unlock() {
    validate_hierarchy();
    m_.unlock();
    restore_hierarchy();
  }

  bool try_lock() {
    validate_hierarchy();
    if (!m_.try_lock()) {
      return false;
    }
    update_hierarchy();
    return true;
  }

  void validate_hierarchy() {
    if (thread_hierarchy_ < cur_hierarchy_) {
      throw std::logic_error("mutex hierarchy violated.");
    }
  }

  void update_hierarchy() {
    prev_hierarchy_ = thread_hierarchy_;
    thread_hierarchy_ = cur_hierarchy_;
  }

  void restore_hierarchy() {
    thread_hierarchy_ = prev_hierarchy_;
  }

private:
  const int cur_hierarchy_;
  int prev_hierarchy_;
  static thread_local int thread_hierarchy_;
  std::mutex m_;
};
thread_local int hierarchy_mutex::thread_hierarchy_ = INT_MAX;
