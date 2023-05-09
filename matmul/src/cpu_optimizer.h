#pragma once
#include "utils.h"
#include "baseline.h"

// for better transpose algorithm, see:
// https://stackoverflow.com/questions/16737298/what-is-the-fastest-way-to-transpose-a-matrix-in-c
template<typename T>
void transpose(std::shared_ptr<T> src, std::shared_ptr<T> dst, int row, int col) {
  for (int i = 0; i < row * col; ++i) {
    int row_idx = i / row;
    int col_idx = i % row;
    // dst[i] --> dst[row_idx, col_idx] --> src[col_idx, row_idx]
    dst[i] = src[col_idx * col + row_idx];
  }
}

template<typename T>
void matmul_transpose(std::shared_ptr<T> lhs, std::shared_ptr<T> rhs, std::shared_ptr<T> res, int M, int N, int K) {
  // lhs(M*K) * rhs(K*N) = res(M*N)
  auto trans_rhs = std::shared_ptr<T>(new std::remove_extent_t<T>[N*K]);
  transpose(rhs, trans_rhs, K, N);
  for (int i = 0; i < M; ++i) {
    for (int j = 0; j < N; ++j) {
      std::remove_extent_t<T> sum = 0;
      for (int k = 0; k < K; ++k) {
        // sum += lhs[i, k] * rhs[k, j];
        sum += lhs[i * K + k] * trans_rhs[j * K + k];
      }
      res[i * N + j] = sum;
    }
  }
}
