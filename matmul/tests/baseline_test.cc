#include "gtest/gtest.h"
#include "baseline.h"
#include "cpu_optimizer.h"
#include "gpu_optimizer.h"
#include "utils.h"

TEST(TestUtils, TestBaseLine) {
  constexpr int M = 4;
  constexpr int N = 4;
  constexpr int K = 4;
  std::shared_ptr<float[]> lhs(new float[M*K]);
  std::shared_ptr<float[]> rhs(new float[K*N]);
  std::shared_ptr<float[]> res(new float[M*N]);

  fullfill_num(lhs, M*K, 1);
  fullfill_num(rhs, K*N, 2);

  Timer t;
  matmul_baseline(lhs, rhs, res, M, N, K);
  double dur = t.tok();
  auto gflops = get_matmul_GFLOPS(M, K, N, dur);

  for (int i = 0; i < M*N; ++i) {
    EXPECT_TRUE(res[i] == 8);
  }
}

TEST(TestUtils, TestTranspose) {
  constexpr int M = 5;
  constexpr int N = 6;
  constexpr int K = 7;
  std::shared_ptr<float[]> lhs(new float[M*K]);
  std::shared_ptr<float[]> rhs(new float[K*N]);
  std::shared_ptr<float[]> golden(new float[M*N]);
  std::shared_ptr<float[]> res(new float[M*N]);

  fullfill_rand(lhs, M*K);
  fullfill_rand(rhs, K*N);

  matmul_baseline(lhs, rhs, golden, M, N, K);
  matmul_transpose(lhs, rhs, res, M, N, K);

  for (int i = 0; i < M*N; ++i) {
    EXPECT_TRUE(res[i] == golden[i]);
  }
}

TEST(TestUtils, TestBlock) {
  constexpr int M = 5;
  constexpr int N = 6;
  constexpr int K = 7;
  std::shared_ptr<float[]> lhs(new float[M*K]);
  std::shared_ptr<float[]> rhs(new float[K*N]);
  std::shared_ptr<float[]> golden(new float[M*N]);
  std::shared_ptr<float[]> res(new float[M*N]);

  fullfill_rand(lhs, M*K);
  fullfill_rand(rhs, K*N);
  fullfill_num(res, M*N, 0);

  matmul_baseline(lhs, rhs, golden, M, N, K);
  matmul_block(lhs, rhs, res, M, N, K);

  /* print_matrix(res, M*N); */
  /* print_matrix(golden, M*N); */
  for (int i = 0; i < M*N; ++i) {
    EXPECT_TRUE(res[i] == golden[i]);
  }
}

TEST(TestUtils, TestUnroll) {
  constexpr int M = 6;
  constexpr int N = 7;
  constexpr int K = 8;
  std::shared_ptr<float[]> lhs(new float[M*K]);
  std::shared_ptr<float[]> rhs(new float[K*N]);
  std::shared_ptr<float[]> golden(new float[M*N]);
  std::shared_ptr<float[]> res(new float[M*N]);

  fullfill_rand(lhs, M*K);
  fullfill_rand(rhs, K*N);
  fullfill_num(res, M*N, 0);

  matmul_baseline(lhs, rhs, golden, M, N, K);
  matmul_unroll(lhs, rhs, res, M, N, K);

  /* print_matrix(res, M*N); */
  /* print_matrix(golden, M*N); */
  for (int i = 0; i < M*N; ++i) {
    EXPECT_FLOAT_EQ(res[i], golden[i]);
  }
}

TEST(TestUtils, TestBlockUnroll) {
  constexpr int M = 32;
  constexpr int N = 32;
  constexpr int K = 32;
  std::shared_ptr<float[]> lhs(new float[M*K]);
  std::shared_ptr<float[]> rhs(new float[K*N]);
  std::shared_ptr<float[]> golden(new float[M*N]);
  std::shared_ptr<float[]> res(new float[M*N]);

  fullfill_rand(lhs, M*K);
  fullfill_rand(rhs, K*N);
  fullfill_num(res, M*N, 0);

  matmul_baseline(lhs, rhs, golden, M, N, K);
  matmul_block_unroll(lhs, rhs, res, M, N, K);

  /* print_matrix(res, M*N); */
  /* print_matrix(golden, M*N); */
  for (int i = 0; i < M*N; ++i) {
    EXPECT_FLOAT_EQ(res[i], golden[i]);
  }
}

TEST(TestUtils, TestSSE) {
  constexpr int M = 4;
  constexpr int N = 4;
  constexpr int K = 4;
  std::shared_ptr<float[]> lhs(new float[M*K]);
  std::shared_ptr<float[]> rhs(new float[K*N]);
  std::shared_ptr<float[]> golden(new float[M*N]);
  std::shared_ptr<float[]> res(new float[M*N]);

  fullfill_rand(lhs, M*K);
  fullfill_rand(rhs, K*N);
  fullfill_num(res, M*N, 0);

  matmul_baseline(lhs, rhs, golden, M, N, K);
  matmul_sse(lhs, rhs, res, M, N, K);

  /* print_matrix(res, M*N); */
  /* print_matrix(golden, M*N); */
  for (int i = 0; i < M*N; ++i) {
    EXPECT_FLOAT_EQ(res[i], golden[i]);
  }
}

/* TEST(TestUtils, TestAVX512) { */
/*   constexpr int M = 16; */
/*   constexpr int N = 4; */
/*   constexpr int K = 4; */
/*   std::shared_ptr<float[]> lhs(new float[M*K]); */
/*   std::shared_ptr<float[]> rhs(new float[K*N]); */
/*   std::shared_ptr<float[]> golden(new float[M*N]); */
/*   std::shared_ptr<float[]> res(new float[M*N]); */

/*   fullfill_rand(lhs, M*K); */
/*   fullfill_rand(rhs, K*N); */
/*   fullfill_num(res, M*N, 0); */

/*   matmul_baseline(lhs, rhs, golden, M, N, K); */
/*   matmul_avx512(lhs, rhs, res, M, N, K); */

/*   /1* print_matrix(res, M*N); *1/ */
/*   /1* print_matrix(golden, M*N); *1/ */
/*   for (int i = 0; i < M*N; ++i) { */
/*     EXPECT_FLOAT_EQ(res[i], golden[i]); */
/*   } */
/* } */

TEST(TestUtils, TestCUBLAS) {
  constexpr int M = 33;
  constexpr int N = 34;
  constexpr int K = 35;
  std::shared_ptr<float[]> lhs(new float[M*K]);
  std::shared_ptr<float[]> rhs(new float[K*N]);
  std::shared_ptr<float[]> golden(new float[M*N]);
  std::shared_ptr<float[]> res(new float[M*N]);

  fullfill_rand(lhs, M*K);
  fullfill_rand(rhs, K*N);
  fullfill_num(res, M*N, 0);

  matmul_baseline(lhs, rhs, golden, M, N, K);
  matmul_cublas(lhs, rhs, res, M, N, K);

  /* print_matrix(res, M*N); */
  /* print_matrix(golden, M*N); */
  for (int i = 0; i < M*N; ++i) {
    EXPECT_FLOAT_EQ(res[i], golden[i]);
  }
}

TEST(TestUtils, TestCUBasic) {
  constexpr int M = 32;
  constexpr int N = 32;
  constexpr int K = 32;
  std::shared_ptr<float[]> lhs(new float[M*K]);
  std::shared_ptr<float[]> rhs(new float[K*N]);
  std::shared_ptr<float[]> golden(new float[M*N]);
  std::shared_ptr<float[]> res(new float[M*N]);

  fullfill_rand(lhs, M*K);
  fullfill_rand(rhs, K*N);
  fullfill_num(res, M*N, 0);

  matmul_baseline(lhs, rhs, golden, M, N, K);
  matmul_cuda_naive(lhs, rhs, res, M, N, K);

  /* print_matrix(res, M*N); */
  /* print_matrix(golden, M*N); */
  for (int i = 0; i < M*N; ++i) {
    EXPECT_FLOAT_EQ(res[i], golden[i]);
  }
}

TEST(TestUtils, TestCUTranspose) {
  constexpr int M = 32;
  constexpr int N = 32;
  constexpr int K = 32;
  std::shared_ptr<float[]> lhs(new float[M*K]);
  std::shared_ptr<float[]> rhs(new float[K*N]);
  std::shared_ptr<float[]> golden(new float[M*N]);
  std::shared_ptr<float[]> res(new float[M*N]);

  fullfill_rand(lhs, M*K);
  fullfill_rand(rhs, K*N);
  fullfill_num(res, M*N, 0);

  matmul_baseline(lhs, rhs, golden, M, N, K);
  matmul_cuda_transpose(lhs, rhs, res, M, N, K);

  /* print_matrix(res, M*N); */
  /* print_matrix(golden, M*N); */
  for (int i = 0; i < M*N; ++i) {
    EXPECT_FLOAT_EQ(res[i], golden[i]);
  }
}
