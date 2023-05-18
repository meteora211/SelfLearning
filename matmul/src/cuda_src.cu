#include <cuda_runtime.h>
#include "gpu_optimizer.h"
#include <cublas_v2.h>
#define OFFSET(row, col, ld) ((row) * (ld) + (col))
#define FLOAT4(pointer) (reinterpret_cast<float4*>(&(pointer))[0])

double cuda_executor(void(*cuda_func)(float *, float *, float *, const int, const int, const int),
                   std::shared_ptr<float[]> lhs,
                   std::shared_ptr<float[]> rhs,
                   std::shared_ptr<float[]> res,
                   int M, int N, int K,
                   dim3 grid,
                   dim3 block) {
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

  cudaEvent_t start, end;
  cudaEventCreate(&start);
  cudaEventCreate(&end);
  cudaEventRecord(start);
  cuda_func<<<grid, block>>>(lhs_device, rhs_device, res_device, M, N, K);

  cudaEventRecord(end);
  cudaEventSynchronize(end);

  float msec;
  cudaEventElapsedTime(&msec, start, end);
  auto gflops = get_matmul_GFLOPS(M, N, K,msec/1000);

  cudaMemcpy(res.get(), res_device, res_size, cudaMemcpyDeviceToHost);


  cudaFree(lhs_device);
  cudaFree(rhs_device);
  cudaFree(res_device);
  return gflops;
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

__global__ void cuda_block(float* lhs, float* rhs, float* res,
    const int M, const int N, const int K) {
  int i = blockIdx.y * blockDim.y + threadIdx.y;
  int j = blockIdx.x * blockDim.x + threadIdx.x;
  int bi = threadIdx.y;
  int bj = threadIdx.x;
  const int block_size = 32;
  __shared__ float block_lhs[block_size * block_size];
  __shared__ float block_rhs[block_size * block_size];

  float sum = 0.0;

  for (int k = 0; k < K; k+=block_size) {
    if ((i < M) && ((bj + k) < K)) {
      block_lhs[bi * block_size + bj] = lhs[i * N + (bj + k)];
    } else {
      block_lhs[bi * block_size + bj] = 0;
    }

    if ((j < N) && ((bi + k) < K)) {
      block_rhs[bi * block_size + bj] = rhs[(bi + k) * K + j];
    } else {
      block_rhs[bi * block_size + bj] = 0;
    }

    __syncthreads();

    for (int bk = 0; bk < block_size; ++bk) {
      sum += block_lhs[bi * block_size + bk] * block_rhs[bk * block_size + bj];
    }

    __syncthreads();
  }

  if (i < M && j < N) {
    res[i * N + j] = sum;
  }
}

__global__ void mySgemmV1Aligned(
    float * __restrict__ a, float * __restrict__ b, float * __restrict__ c,
    const int M, const int N, const int K) {

    const int BM = 128;
    const int BN = 128;
    const int BK = 8;
    const int TM = 8;
    const int TN = 8;

    const int bx = blockIdx.x;
    const int by = blockIdx.y;
    const int tx = threadIdx.x;
    const int ty = threadIdx.y;
    const int tid = ty * blockDim.x + tx;

    __shared__ float s_a[BM][BK];
    __shared__ float s_b[BK][BN];

    float r_c[TM][TN] = {0.0};

    int load_a_smem_m = tid >> 1;
    int load_a_smem_k = (tid & 1) << 2;
    int load_b_smem_k = tid >> 5;
    int load_b_smem_n = (tid & 31) << 2;

    int load_a_gmem_m = by * BM + load_a_smem_m;
    int load_b_gmem_n = bx * BN + load_b_smem_n;

    for (int bk = 0; bk < (K + BK - 1) / BK; bk++) {
        int load_a_gmem_k = bk * BK + load_a_smem_k;
        int load_a_gmem_addr = OFFSET(load_a_gmem_m, load_a_gmem_k, K);
        FLOAT4(s_a[load_a_smem_m][load_a_smem_k]) = FLOAT4(a[load_a_gmem_addr]);
        int load_b_gmem_k = bk * BK + load_b_smem_k;
        int load_b_gmem_addr = OFFSET(load_b_gmem_k, load_b_gmem_n, N);
        FLOAT4(s_b[load_b_smem_k][load_b_smem_n]) = FLOAT4(b[load_b_gmem_addr]);

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < BK; k++) {
            #pragma unroll
            for (int m = 0; m < TM; m++) {
                #pragma unroll
                for (int n = 0; n < TN; n++) {
                    int comp_a_smem_m = ty * TM + m;
                    int comp_b_smem_n = tx * TN + n;
                    r_c[m][n] += s_a[comp_a_smem_m][k] * s_b[k][comp_b_smem_n];
                }
            }
        }

        __syncthreads();
    }

    #pragma unroll
    for (int i = 0; i < TM; i++) {
        int store_c_gmem_m = by * BM + ty * TM + i;
        #pragma unroll
        for (int j = 0; j < TN; j += 4) {
            int store_c_gmem_n = bx * BN + tx * TN + j;
            int store_c_gmem_addr = OFFSET(store_c_gmem_m, store_c_gmem_n, N);
            FLOAT4(c[store_c_gmem_addr]) = FLOAT4(r_c[i][j]);
        }
    }
}

double matmul_cublas(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K) {
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
  cudaEvent_t start, end;
  cudaEventCreate(&start);
  cudaEventCreate(&end);
  cudaEventRecord(start);

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

  cudaEventRecord(end);
  cudaEventSynchronize(end);

  float msec;
  cudaEventElapsedTime(&msec, start, end);
  auto gflops = get_matmul_GFLOPS(M, N, K,msec/1000);

  cudaMemcpy(res.get(), res_device, res_size, cudaMemcpyDeviceToHost);


  cudaFree(lhs_device);
  cudaFree(rhs_device);
  cudaFree(res_device);
  return gflops;
}

double matmul_cuda_naive(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K) {
  const int BM = 32;
  const int BN = 32;

  dim3 grid((N + BN - 1) / BN, (M + BM - 1) / BM);
  dim3 block(BN, BM);
  return cuda_executor(cuda_basic, rhs, lhs, res, M, N, K, grid, block);
}

double matmul_cuda_transpose(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K) {
  const int BM = 32;
  const int BN = 32;

  dim3 grid((N + BN - 1) / BN, (M + BM - 1) / BM);
  dim3 block(BN, BM);
  return cuda_executor(cuda_transpose, rhs, lhs, res, M, N, K, grid, block);
}

double matmul_cuda_block(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K) {
  const int BM = 32;
  const int BN = 32;

  dim3 grid((N + BN - 1) / BN, (M + BM - 1) / BM);
  dim3 block(BN, BM);
  return cuda_executor(cuda_block, rhs, lhs, res, M, N, K, grid, block);
}

/* void matmul_cuda_copied_code(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K) { */
/*   const int BM = 128, BN = 128, TM = 8, TN = 8; */
/*   dim3 grid((N + BN - 1) / BN, (M + BM - 1) / BM); */
/*   dim3 block(BN / TN, BM / TM); */

/*   cuda_executor(mySgemmV1Aligned, rhs, lhs, res, M, N, K, grid, block); */
/* } */
