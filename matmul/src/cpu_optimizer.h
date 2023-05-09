#pragma once
#include "utils.h"
#include "baseline.h"
#include <new>

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

template<typename T>
void matmul_block(std::shared_ptr<T> lhs, std::shared_ptr<T> rhs, std::shared_ptr<T> res, int M, int N, int K) {
  // lhs(M*K) * rhs(K*N) = res(M*N)
  constexpr int block_size = 32;

  int MB = M / block_size + 1;
  int NB = N / block_size + 1;
  int KB = K / block_size + 1;

  for (int i = 0; i < MB; ++i) {
    for (int j = 0; j < NB; ++j) {
      std::remove_extent_t<T>sum_b[block_size * block_size];
      for (int k = 0; k < KB; ++k) {
        // res_b = lhs_b[i * K + k] * rhs_b[k * N + j];
        for (int bi = 0; bi < block_size; ++bi) {
          for (int bj = 0; bj < block_size; ++bj) {
            std::remove_extent_t<T> sum = 0;
            for (int bk = 0; bk < block_size; ++bk) {
              auto lhs_row = (bi + i * block_size);
              auto lhs_col = (bk + k * block_size);
              auto rhs_col = (bj + j * block_size);

              if (lhs_row < M && lhs_col < K && rhs_col < N) {
                auto lhs_idx = lhs_row * K + lhs_col;
                auto rhs_idx = lhs_col * N + rhs_col;
                sum += lhs[lhs_idx] * rhs[rhs_idx];
              }
            }
            // res_b[bi * block_size + bj] = sum;
            sum_b[bi * block_size + bj] += sum;
          }
        }
        // sum_b += res_b;
      }
      for (int bi = 0; bi < block_size; ++bi) {
        for (int bj = 0; bj < block_size; ++bj) {
          auto res_row = (bi + i * block_size);
          auto res_col = (bj + j * block_size);
          if (res_row < M && res_col < N) {
            auto res_idx = res_row * N + res_col;
            res[res_idx] = sum_b[bi * block_size + bj];
          }
        }
      }

    }
  }
}
