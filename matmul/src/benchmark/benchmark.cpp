#include "baseline.h"
#include "cpu_optimizer.h"
#include "utils.h"
#include <ranges>
#include <functional>

int main() {
  std::cout << "BENCHMARK START" << std::endl;

  auto step = [](int i){return i % 20 == 0 && i > 0;};

  std::function<void(std::shared_ptr<float[]>, std::shared_ptr<float[]>, std::shared_ptr<float[]>, int, int, int)> fn;

  auto benchmark = [&fn](int i) {
    const int nelm = i*i;
    std::shared_ptr<float[]> lhs(new float[nelm]);
    std::shared_ptr<float[]> rhs(new float[nelm]);
    std::shared_ptr<float[]> res(new float[nelm]);

    fullfill_rand(lhs, nelm);
    fullfill_rand(rhs, nelm);
    fullfill_rand(res, nelm);

    Timer t;
    fn(lhs, rhs, res, i, i, i);
    auto dur = t.tok();
    auto gflops = get_matmul_GFLOPS(i, i, i, dur);
    return std::pair(i, gflops);
  };

  fn = matmul_baseline<float[]>;

  // TODO: std::views::for_each to simplify code.
  for (const auto&& [i, gflops] : std::views::iota(0, 1000) | std::views::filter(step) | std::views::transform(benchmark)) {
    std::cout << "size: " << i << " baseline gflops: " << gflops << std::endl;
  }

  fn = matmul_transpose<float[]>;

  for (const auto&& [i, gflops] : std::views::iota(0, 1000) | std::views::filter(step) | std::views::transform(benchmark)) {
    std::cout << "size: " << i << " transpose gflops: " << gflops << std::endl;
  }

  fn = matmul_block<float[]>;

  for (const auto&& [i, gflops] : std::views::iota(0, 1000) | std::views::filter(step) | std::views::transform(benchmark)) {
    std::cout << "size: " << i << " block gflops: " << gflops << std::endl;
  }

  std::cout << "BENCHMARK END" << std::endl;
}
