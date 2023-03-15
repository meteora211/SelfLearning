#include <iostream>
#include <type_traits>

// constinit, consteval, constexpr
// See more in https://www.cppstories.com/2022/const-options-cpp20/

consteval int sum_eval(int v1, int v2) {
  return v1 + v2;
}

constexpr int sum_expr(int v1, int v2) {
  return v1 + v2;
}

constinit int global = sum_eval(10, 10);
// Compile error here. constinit can not be used in constant expression
// ‘constinit’ variable ‘another’ does not have a constant initializer
// constinit int another = global;


// fold expressions
// (pack op ... [op init])
// ([init op] ... pack op)
template <typename... Args>
auto sum(Args... args) {
  return (args + ...);
//        ^^  ^^
//    ( pack  op ... )
}

template <typename... Args>
auto sum_with_zero(Args... args) {
  return (0 + ... + args);
//        ^^      ^^  ^^
//    (  init ... op  pack )
}

template <typename... Args>
auto lfold(Args... args) {
  return (10 - ... - args);
//        ^^      ^^  ^^
//    (  init ... op  pack )
}

template <typename... Args>
auto rfold(Args... args) {
  return (args - ... - 10);
//        ^^   ^^      ^^
//    (  pack  op ...  init )
}

int main() {
  constexpr auto val = 100;
  static_assert(sum_eval(val, val) == 200);
  static_assert(sum_expr(val, val) == 2* val);

  int a = 10;
  int b = sum_expr(a, 10);
  // Compile error: error: the value of ‘a’ is not usable in a constant expression
  // int c = sum_eval(a, 10);

  std::cout << sum(1,2,3,4) << std::endl;
  std::cout << sum_with_zero() << std::endl;
  std::cout << lfold(1,2,3,4) << std::endl; // ((((10 - 1) - 2) - 3) - 4) = 0
  std::cout << rfold(1,2,3,4) << std::endl; // (1 - (2 - (3 - (4 - 10)))) = 8
}
