#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <chrono>
#include <type_traits>
constexpr int M = 4;
constexpr int N = 4;
constexpr int K = 4;

template<typename T>
void fullfill_rand(std::shared_ptr<T> input, int nelm) {
  std::srand(std::time(nullptr)); // use current time as seed for random generator
  for (int i = 0; i < nelm; ++i) {
    input[i] = std::rand() / static_cast<std::remove_extent_t<T>>(RAND_MAX);
  }
}

template<typename T>
void print_matrix(std::shared_ptr<T> input, int nelm) {
  for (int i = 0; i < nelm; ++i) {
    std::cout << input[i] << " ";
  }
  std::cout << std::endl;
}

template<typename T>
void matmul_baseline(std::shared_ptr<T> lhs, std::shared_ptr<T> rhs, std::shared_ptr<T> res, int M, int N, int K) {
  // lhs(M*K) * rhs(K*N) = res(M*N)
  for (int i = 0; i < M; ++i) {
    for (int j = 0; j < N; ++j) {
      std::remove_extent_t<T> sum = 0;
      for (int k = 0; k < K; ++k) {
        // sum += lhs[i, k] * rhs[k, j];
        sum += lhs[i * K + k] * rhs[k * K + j];
      }
      res[i * N + j] = sum;
    }
  }
}

int main() {
  std::shared_ptr<float[]> lhs(new float[M*K]);
  std::shared_ptr<float[]> rhs(new float[K*N]);
  std::shared_ptr<float[]> res(new float[M*N]);

  fullfill_rand(lhs, M*K);
  fullfill_rand(rhs, K*N);

  matmul_baseline(lhs, rhs, res, M, N, K);

  print_matrix(lhs, M*K);
  print_matrix(rhs, K*N);
  print_matrix(res, M*N);
}
