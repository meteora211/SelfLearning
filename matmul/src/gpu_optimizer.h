#pragma once
#include "utils.h"
#include "baseline.h"

void cuda_executor(void(*cuda_func)(float *, float *, float *, const int, const int, const int),
                   std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K);

void matmul_cublas(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K);

void matmul_cuda_naive(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K);

void matmul_cuda_transpose(std::shared_ptr<float[]> lhs, std::shared_ptr<float[]> rhs, std::shared_ptr<float[]> res, int M, int N, int K);
