#pragma once
#include <chrono>

class Timer {
public:
  Timer() : timer_(std::chrono::high_resolution_clock::now()) {}
  double tok() {
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - timer_;
    return diff.count();
  }

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> timer_;
};

double get_matmul_FLOPs(int M, int K, int N);
double get_matmul_GFLOPS(int M, int K, int N, double time);
