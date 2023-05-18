#pragma once
#include "utils.h"
#include "baseline.h"

// Some useful repo:
// https://github.com/Liu-xiandong/How_to_optimize_in_GPU
// https://github.com/nicolaswilde/cuda-tensorcore-hgemm
// https://github.com/nicolaswilde/cuda-sgemm

double cuda_executor(void(*cuda_func)(float *, float *, float *, const int, const int, const int),
                   std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K);

double matmul_cublas(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K);

double matmul_cuda_naive(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K);

double matmul_cuda_transpose(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K);

double matmul_cuda_block(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K);
