#include "baseline.h"
#include "cpu_optimizer.h"
#include "utils.h"
#include <ranges>
#include <functional>
#include <string>
#include <fmt/core.h>

using namespace std::placeholders;

int main() {
  std::cout << "BENCHMARK START" << std::endl;

  auto step = [](int i){return i % 20 == 0 && i > 0;};

  typedef std::function<void(std::shared_ptr<float[]>, std::shared_ptr<float[]>, std::shared_ptr<float[]>, int, int, int)> Fn;
  Fn fn;

  auto benchmark = [&](Fn fn, int i) {
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

  auto run = [&](Fn fn, const std::string& name){
    for (const auto&& [i, gflops] : std::views::iota(0, 1000) | std::views::filter(step) | std::views::transform([&](int i){ return benchmark(fn, i); })) {
      std::cout << fmt::format("size: {}, {} gflops: {}.\n", i, name, gflops);
    }
  };

  run(matmul_baseline<float[]>, "baseline");
  run(matmul_transpose<float[]>, "tranpose");
  run(matmul_block<float[]>, "block");
  run(matmul_unroll, "unroll");
  run(matmul_block_unroll, "block unroll");
  run(matmul_sse, "SSE");


  std::cout << "BENCHMARK END" << std::endl;
}
