#include <cuda_runtime.h>
#include "gpu_optimizer.h"

void cuda_executor(void(*cuda_func)(float *, float *, float *, const int, const int, const int),
                   std::shared_ptr<float[]> lhs,
                   std::shared_ptr<float[]> rhs,
                   std::shared_ptr<float[]> res,
                   int M, int N, int K) {
  size_t lhs_size = M * K * sizeof(float);
  size_t rhs_size = K * N * sizeof(float);
  size_t res_size = M * N * sizeof(float);

  float* lhs_device, *rhs_device, *res_device;
  cudaMalloc(&lhs_device, lhs_size);
  cudaMalloc(&rhs_device, rhs_size);
  cudaMalloc(&res_device, res_size);

  cudaMemcpy(lhs_device, lhs.get(), lhs_size, cudaMemcpyHostToDevice);
  cudaMemcpy(rhs_device, rhs.get(), rhs_size, cudaMemcpyHostToDevice);
  cudaMemcpy(res_device, res.get(), res_size, cudaMemcpyHostToDevice);

  const int BM = 32;
  const int BN = 32;

  dim3 block(BN, BM);
  dim3 grid((N + BN - 1) / BN, (M + BM - 1) / BM);
  cuda_func<<<grid, block>>>(lhs_device, rhs_device, res_device, M, N, K);

  cudaMemcpy(res.get(), res_device, res_size, cudaMemcpyDeviceToHost);


  cudaFree(lhs_device);
  cudaFree(rhs_device);
  cudaFree(res_device);
}


constexpr int block_size = 32;

__global__ void cuda_basic(
    float* lhs, float* rhs, float* res,
    const int M, const int N, const int K) {
  int i = blockIdx.y * blockDim.y + threadIdx.y;
  int j = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < M && j < N) {
    float sum = 0.0;
    for (int k = 0; k < K; k++) {
        sum += lhs[i * K + k] * rhs[k * N + j];
    }
    res[i * N + j] = sum;
  }
}

// From 15-418:
// CUDA threads numbered within block in ROW-major order:
// X = column number; Y = row number;
// - Threads with same value of Y map to single warp
// - Threads with same value of Y map and consecutive values of X map to consecutive positions in single warp
// - When single warp access consecutive memory locations, do block read or write
// - When single warp access separated memory locations, requires gather(read) or scatter(write)
__global__ void cuda_transpose(
    float* lhs, float* rhs, float* res,
  const int M, const int N, const int K) {
  // i and j(matrix index for result) is mapped to block in inverted way.
  // Note that it doesn't influence the correctness of result but only the perfermance.
  // Threads within a warp:
  // Regular:  res[i, j : j + 32] = lhs[i, :] * rhs[:, j : j + 32]
  // Inverted: res[i : i + 32, j] = lhs[i : i + 32, :] * rhs[:, j]
  // Read/Write in regular match the memory order and can use block read/write
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  int j = blockIdx.y * blockDim.y + threadIdx.y;
  if (i < M && j < N) {
    float sum = 0.0;
    for (int k = 0; k < K; k++) {
        sum += lhs[i * K + k] * rhs[k * N + j];
    }
    res[i * N + j] = sum;
  }
}

void matmul_cuda_naive(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K) {
  cuda_executor(cuda_basic, rhs, lhs, res, M, N, K);
}

void matmul_cuda_transpose(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K) {
  cuda_executor(cuda_transpose, rhs, lhs, res, M, N, K);
}
