#include "baseline.h"
#include "cpu_optimizer.h"
#include "utils.h"
#include <ranges>

int main() {
  std::cout << "BENCHMARK START" << std::endl;

  auto step = [](int i){return i % 20 == 0 && i > 0;};

  for (int i : std::views::iota(0, 1000) | std::views::filter(step)) {
    const int nelm = i*i;
    std::shared_ptr<float[]> lhs(new float[nelm]);
    std::shared_ptr<float[]> rhs(new float[nelm]);
    std::shared_ptr<float[]> res(new float[nelm]);

    fullfill_rand(lhs, nelm);
    fullfill_rand(rhs, nelm);
    fullfill_rand(res, nelm);

    Timer t;
    matmul_baseline(lhs, rhs, res, i, i, i);
    auto dur = t.tok();
    auto gflops = get_matmul_GFLOPS(i, i, i, dur);
    std::cout << "size: " << i << " baseline gflops: " << gflops << std::endl;
  }

  for (int i : std::views::iota(0, 1000) | std::views::filter(step)) {
    const int nelm = i*i;
    std::shared_ptr<float[]> lhs(new float[nelm]);
    std::shared_ptr<float[]> rhs(new float[nelm]);
    std::shared_ptr<float[]> res(new float[nelm]);

    fullfill_rand(lhs, nelm);
    fullfill_rand(rhs, nelm);
    fullfill_rand(res, nelm);

    Timer t;
    matmul_transpose(lhs, rhs, res, i, i, i);
    auto dur = t.tok();
    auto gflops = get_matmul_GFLOPS(i, i, i, dur);
    std::cout << "size: " << i << " baseline gflops: " << gflops << std::endl;
  }

  std::cout << "BENCHMARK END" << std::endl;
}
