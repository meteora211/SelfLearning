#include <cuda_runtime.h>
#include "gpu_optimizer.h"

__global__ void naiveSgemm(
    float* a, float* b, float* c,
    const int M, const int N, const int K) {

    int i = blockIdx.y * blockDim.y + threadIdx.y;
    int j = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < M && j < N) {
        float sum = 0.0;
        for (int k = 0; k < K; k++) {
            sum += a[i * K + k] * b[k * N + j];
        }
        c[i * N + j] = sum;
    }
}

void matmul_cuda_naive(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K) {
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

  naiveSgemm<<<grid, block>>>(lhs_device, rhs_device, res_device, M, N, K);

  cudaMemcpy(res.get(), res_device, res_size, cudaMemcpyDeviceToHost);

  cudaFree(lhs_device);
  cudaFree(rhs_device);
  cudaFree(res_device);
}
