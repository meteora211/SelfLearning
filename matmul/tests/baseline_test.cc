#include "gtest/gtest.h"
#include "baseline.h"
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
