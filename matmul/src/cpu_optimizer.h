#pragma once
#include "utils.h"
#include "baseline.h"
#include <new>
#include <emmintrin.h>
#include <immintrin.h>
#include <fmaintrin.h>

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
#ifdef __cpp_lib_hardware_interference_size
  // block_size = 64 (a cache line size) / 4 (sizeof(float)) / 2 (block for rhs and lhs) = 8
  constexpr size_t block_size = std::hardware_constructive_interference_size / sizeof(std::remove_extent_t<T>) / 2;
#else
  constexpr size_t block_size = 8;
#endif
  auto trans_rhs = std::shared_ptr<T>(new std::remove_extent_t<T>[N*K]);
  transpose(rhs, trans_rhs, K, N);
  // do not clear res for pure speed test
  // fullfill_num(res, M*N, 0);

  for (int i = 0; i < M; i += block_size) {
    for (int j = 0; j < N; j += block_size) {
      for (int k = 0; k < K; k += block_size) {
        // res_b = lhs_b[i * K + k] * rhs_b[k * N + j];
        for (int bi = 0; bi < block_size; ++bi) {
          for (int bj = 0; bj < block_size; ++bj) {
            std::remove_extent_t<T> sum = 0;
            auto i_idx = i + bi;
            auto j_idx = j + bj;
            for (int bk = 0; bk < block_size; ++bk) {
              auto k_idx = k + bk;
              if (i_idx < M && j_idx < N && k_idx < K) {
                // auto rhs_idx = k_idx * N + j_idx;
                // sum += lhs[lhs_idx] * rhs[rhs_idx];
                auto lhs_idx = i_idx * K + k_idx;
                auto rhs_idx = j_idx * K + k_idx;
                sum += lhs[lhs_idx] * trans_rhs[rhs_idx];
              }
            }
            // res_b[bi * block_size + bj] = sum;
            if (i_idx < M && j_idx < N) {
              res[i_idx * N + j_idx] += sum;
            }
          }
        }
      }
    }
  }
}

void matmul_unroll(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K) {
  float tmp00, tmp01, tmp02, tmp03;
  float tmp10, tmp11, tmp12, tmp13;
  float tmp20, tmp21, tmp22, tmp23;
  float tmp30, tmp31, tmp32, tmp33;

  int M_4 = (M >> 2) << 2;
  int N_4 = (N >> 2) << 2;
  for (int i = 0; i < M_4; i+=4) {
    for (int j = 0; j < N_4; j += 4) {
      tmp00 = tmp01 = tmp02 = tmp03 = 0;
      tmp10 = tmp11 = tmp12 = tmp13 = 0;
      tmp20 = tmp21 = tmp22 = tmp23 = 0;
      tmp30 = tmp31 = tmp32 = tmp33 = 0;
      for (int k = 0; k < K; ++k) {
        tmp00 += lhs[(i + 0) * K + k] * rhs[k * N + (j + 0)];
        tmp01 += lhs[(i + 0) * K + k] * rhs[k * N + (j + 1)];
        tmp02 += lhs[(i + 0) * K + k] * rhs[k * N + (j + 2)];
        tmp03 += lhs[(i + 0) * K + k] * rhs[k * N + (j + 3)];
        tmp10 += lhs[(i + 1) * K + k] * rhs[k * N + (j + 0)];
        tmp11 += lhs[(i + 1) * K + k] * rhs[k * N + (j + 1)];
        tmp12 += lhs[(i + 1) * K + k] * rhs[k * N + (j + 2)];
        tmp13 += lhs[(i + 1) * K + k] * rhs[k * N + (j + 3)];
        tmp20 += lhs[(i + 2) * K + k] * rhs[k * N + (j + 0)];
        tmp21 += lhs[(i + 2) * K + k] * rhs[k * N + (j + 1)];
        tmp22 += lhs[(i + 2) * K + k] * rhs[k * N + (j + 2)];
        tmp23 += lhs[(i + 2) * K + k] * rhs[k * N + (j + 3)];
        tmp30 += lhs[(i + 3) * K + k] * rhs[k * N + (j + 0)];
        tmp31 += lhs[(i + 3) * K + k] * rhs[k * N + (j + 1)];
        tmp32 += lhs[(i + 3) * K + k] * rhs[k * N + (j + 2)];
        tmp33 += lhs[(i + 3) * K + k] * rhs[k * N + (j + 3)];
      }
      res[(i + 0) * N + (j + 0)] = tmp00;
      res[(i + 0) * N + (j + 1)] = tmp01;
      res[(i + 0) * N + (j + 2)] = tmp02;
      res[(i + 0) * N + (j + 3)] = tmp03;

      res[(i + 1) * N + (j + 0)] = tmp10;
      res[(i + 1) * N + (j + 1)] = tmp11;
      res[(i + 1) * N + (j + 2)] = tmp12;
      res[(i + 1) * N + (j + 3)] = tmp13;

      res[(i + 2) * N + (j + 0)] = tmp20;
      res[(i + 2) * N + (j + 1)] = tmp21;
      res[(i + 2) * N + (j + 2)] = tmp22;
      res[(i + 2) * N + (j + 3)] = tmp23;

      res[(i + 3) * N + (j + 0)] = tmp30;
      res[(i + 3) * N + (j + 1)] = tmp31;
      res[(i + 3) * N + (j + 2)] = tmp32;
      res[(i + 3) * N + (j + 3)] = tmp33;
    }
  }

  if (M_4 == M && N_4 == N) {
    return;
  }

  for (int i = 0; i < M; ++i) {
    for (int j = N_4; j < N; ++j) {
      float sum = 0;
      for (int k = 0; k < K; ++k) {
        sum += lhs[i * K + k] * rhs[k * N + j];
      }
      res[i * N + j] = sum;
    }
  }
  for (int i = M_4; i < M; ++i) {
    for (int j = 0; j < N_4; ++j) {
      float sum = 0;
      for (int k = 0; k < K; ++k) {
        sum += lhs[i * K + k] * rhs[k * N + j];
      }
      res[i * N + j] = sum;
    }
  }
}

void matmul_block_unroll(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K) {
  float tmp00, tmp01, tmp02, tmp03;
  float tmp10, tmp11, tmp12, tmp13;
  float tmp20, tmp21, tmp22, tmp23;
  float tmp30, tmp31, tmp32, tmp33;
  constexpr size_t block_size = 8;
  auto trans_rhs = std::shared_ptr<float[]>(new float[N*K]);
  transpose(rhs, trans_rhs, K, N);
  // do not clear res for pure speed test
  // fullfill_num(res, M*N, 0);

  // FIXME: boundary check in innerloop is skipped.
  for (int i = 0; i < M; i += block_size) {
    for (int j = 0; j < N; j += block_size) {
      for (int k = 0; k < K; k += block_size) {
        // res_b = lhs_b[i * K + k] * rhs_b[k * N + j];
        for (int bi = 0; bi < block_size; bi+=4) {
          for (int bj = 0; bj < block_size; bj+=4) {
            tmp00 = tmp01 = tmp02 = tmp03 = 0;
            tmp10 = tmp11 = tmp12 = tmp13 = 0;
            tmp20 = tmp21 = tmp22 = tmp23 = 0;
            tmp30 = tmp31 = tmp32 = tmp33 = 0;
            auto i_idx = i + bi;
            auto j_idx = j + bj;
            for (int bk = 0; bk < block_size; ++bk) {
              auto k_idx = k + bk;
              if (i_idx < M && j_idx < N && k_idx < K) {
                // auto rhs_idx = k_idx * N + j_idx;
                // sum += lhs[lhs_idx] * rhs[rhs_idx];
                auto lhs_idx_0 = (i_idx + 0) * K + k_idx;
                auto lhs_idx_1 = (i_idx + 1) * K + k_idx;
                auto lhs_idx_2 = (i_idx + 2) * K + k_idx;
                auto lhs_idx_3 = (i_idx + 3) * K + k_idx;

                auto rhs_idx_0 = (j_idx + 0) * K + k_idx;
                auto rhs_idx_1 = (j_idx + 1) * K + k_idx;
                auto rhs_idx_2 = (j_idx + 2) * K + k_idx;
                auto rhs_idx_3 = (j_idx + 3) * K + k_idx;

                /* sum += lhs[lhs_idx] * trans_rhs[rhs_idx]; */
                tmp00 += lhs[lhs_idx_0] * trans_rhs[rhs_idx_0];
                tmp01 += lhs[lhs_idx_0] * trans_rhs[rhs_idx_1];
                tmp02 += lhs[lhs_idx_0] * trans_rhs[rhs_idx_2];
                tmp03 += lhs[lhs_idx_0] * trans_rhs[rhs_idx_3];

                tmp10 += lhs[lhs_idx_1] * trans_rhs[rhs_idx_0];
                tmp11 += lhs[lhs_idx_1] * trans_rhs[rhs_idx_1];
                tmp12 += lhs[lhs_idx_1] * trans_rhs[rhs_idx_2];
                tmp13 += lhs[lhs_idx_1] * trans_rhs[rhs_idx_3];

                tmp20 += lhs[lhs_idx_2] * trans_rhs[rhs_idx_0];
                tmp21 += lhs[lhs_idx_2] * trans_rhs[rhs_idx_1];
                tmp22 += lhs[lhs_idx_2] * trans_rhs[rhs_idx_2];
                tmp23 += lhs[lhs_idx_2] * trans_rhs[rhs_idx_3];

                tmp30 += lhs[lhs_idx_3] * trans_rhs[rhs_idx_0];
                tmp31 += lhs[lhs_idx_3] * trans_rhs[rhs_idx_1];
                tmp32 += lhs[lhs_idx_3] * trans_rhs[rhs_idx_2];
                tmp33 += lhs[lhs_idx_3] * trans_rhs[rhs_idx_3];
              }
            }
            // res_b[bi * block_size + bj] = sum;
            if (i_idx < M && j_idx < N) {
              res[(i_idx + 0) * N + (j_idx + 0)] += tmp00;
              res[(i_idx + 0) * N + (j_idx + 1)] += tmp01;
              res[(i_idx + 0) * N + (j_idx + 2)] += tmp02;
              res[(i_idx + 0) * N + (j_idx + 3)] += tmp03;

              res[(i_idx + 1) * N + (j_idx + 0)] += tmp10;
              res[(i_idx + 1) * N + (j_idx + 1)] += tmp11;
              res[(i_idx + 1) * N + (j_idx + 2)] += tmp12;
              res[(i_idx + 1) * N + (j_idx + 3)] += tmp13;

              res[(i_idx + 2) * N + (j_idx + 0)] += tmp20;
              res[(i_idx + 2) * N + (j_idx + 1)] += tmp21;
              res[(i_idx + 2) * N + (j_idx + 2)] += tmp22;
              res[(i_idx + 2) * N + (j_idx + 3)] += tmp23;

              res[(i_idx + 3) * N + (j_idx + 0)] += tmp30;
              res[(i_idx + 3) * N + (j_idx + 1)] += tmp31;
              res[(i_idx + 3) * N + (j_idx + 2)] += tmp32;
              res[(i_idx + 3) * N + (j_idx + 3)] += tmp33;
            }
          }
        }
      }
    }
  }
}

void matmul_vectorize(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K) {
    __m128 t0, t1;
    __m128 res0, res1, res2, res3;
    for(int i=0;i<M;i += 4){
    	for(int j=0;j<N;j += 4){
            res0 = _mm_setzero_ps();
            res1 = _mm_setzero_ps();
            res2 = _mm_setzero_ps();
            res3 = _mm_setzero_ps();
    		for(int k=0; k<K; k++){
                t1 = _mm_load_ps(&rhs[k*N+j]);

                t0 = _mm_load_ps1(&lhs[i*K+k]);
                res0 = _mm_fmadd_ps(t0, t1, res0);

                t0 = _mm_load_ps1(&lhs[(i+1)*K+k]);
                res1 = _mm_fmadd_ps(t0, t1, res1);

                t0 = _mm_load_ps1(&lhs[(i+2)*K+k]);
                res2 = _mm_fmadd_ps(t0, t1, res2);

                t0 = _mm_load_ps1(&lhs[(i+3)*K+k]);
                res3 = _mm_fmadd_ps(t0, t1, res3);
    		}
            _mm_store_ps(&res[i*N+j], res0);
            _mm_store_ps(&res[(i+1)*N+j], res1);
            _mm_store_ps(&res[(i+2)*N+j], res2);
            _mm_store_ps(&res[(i+3)*N+j], res3);
    	}
    }
}
