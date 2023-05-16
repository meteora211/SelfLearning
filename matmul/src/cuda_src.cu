#include <cuda_runtime.h>
#include "gpu_optimizer.h"
#include <cublas_v2.h>

constexpr int inner_block = 32;

void cuda_executor(void(*cuda_func)(float *, float *, float *, const int, const int, const int),
                   std::shared_ptr<float[]> lhs,
                   std::shared_ptr<float[]> rhs,
                   std::shared_ptr<float[]> res,
                   int M, int N, int K,
                   dim3 block,
                   dim3 grid) {
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

  cuda_func<<<grid, block>>>(lhs_device, rhs_device, res_device, M, N, K);

  cudaMemcpy(res.get(), res_device, res_size, cudaMemcpyDeviceToHost);


  cudaFree(lhs_device);
  cudaFree(rhs_device);
  cudaFree(res_device);
}


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

/* __global__ void cuda_block(float* lhs, float* rhs, float* res, */
/*     const int M, const int N, const int K) { */
/*   int i = blockIdx.y * blockDim.y + threadIdx.y; */
/*   int j = blockIdx.x * blockDim.x + threadIdx.x; */
/*   int bi = threadIdx.y; */
/*   int bj = threadIdx.x; */
/*   const int BM = 256; */
/*   const int BN = 256; */
/*   const int BK = 32; */
/*   __shared__ float block_lhs[BM * BK]; */
/*   __shared__ float block_rhs[BK * BN]; */

/*   float block_res[inner_block_size * inner_block_size] = {0}; */


/*   float sum = 0.0; */
/*   for (int k = 0; k < K; k+=BK) { */
/*     block_lhs[bi * inner_block_size + bj] = lhs[(i + )] */
/*     block_rhs[bj * inner_block_size + bi] = rhs[i, k] */

/*     sum += lhs[i * K + k] * rhs[k * N + j]; */
/*   } */
/*   if (i < M && j < N) { */
/*     res[i * N + j] = sum; */
/*   } */
/* } */

void matmul_cublas(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K) {
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

  cublasHandle_t cublas_handle;
  cublasCreate(&cublas_handle);
  float cublas_alpha = 1.0;
  float cublas_beta = 0;
  //https://docs.nvidia.com/cuda/cublas/index.html?highlight=cublasSgemm#cublas-t-gemm
  cublasSgemm(cublas_handle,
              CUBLAS_OP_N,
              CUBLAS_OP_N,
              // TODO: not sure why M/N/K got wrong result
              // M, N, K,
              N, M, K,
              &cublas_alpha,
              rhs_device, N,
              lhs_device, K,
              &cublas_beta,
              res_device, N);

  cudaMemcpy(res.get(), res_device, res_size, cudaMemcpyDeviceToHost);


  cudaFree(lhs_device);
  cudaFree(rhs_device);
  cudaFree(res_device);
}

void matmul_cuda_naive(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K) {
  const int BM = 32;
  const int BN = 32;

  dim3 grid((N + BN - 1) / BN, (M + BM - 1) / BM);
  dim3 block(BN, BM);
  cuda_executor(cuda_basic, rhs, lhs, res, M, N, K, grid, block);
}

void matmul_cuda_transpose(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K) {
  const int BM = 32;
  const int BN = 32;

  dim3 grid((N + BN - 1) / BN, (M + BM - 1) / BM);
  dim3 block(BN, BM);
  cuda_executor(cuda_transpose, rhs, lhs, res, M, N, K, grid, block);
}

/* void matmul_cuda_block(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K) { */
/*   const int BM = 256; */
/*   const int BN = 256; */

/*   dim3 grid((N + BN - 1) / BN, (M + BM - 1) / BM); */
/*   dim3 block(BN / inner_block, BM / inner_block); */
/*   cuda_executor(cuda_block, rhs, lhs, res, M, N, K, grid, block); */
/* } */
