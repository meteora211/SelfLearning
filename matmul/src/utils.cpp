#include "utils.h"

double get_matmul_FLOPs(int M, int K, int N) {
  return static_cast<double>((K * 2) * M * N);
}

double get_matmul_GFLOPS(int M, int K, int N, double time) {
  return get_matmul_FLOPs(M, K, N) / 1024 / 1024 / 1024 / time;
}
