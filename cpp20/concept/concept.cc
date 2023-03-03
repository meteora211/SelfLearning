#include <type_traits>
#include <concepts>
#include <iostream>

struct FooF { using type = float; };
struct FooI { using type = int; };

struct Test {
  int test;
  static int static_test;
  using type = int;
  int operator+(const Test& rhs) {
    return test + rhs.test;
  }
  int operator+(int x) {
    return test + x;
  }
};

template<typename T>
struct Empty{};
template<typename T>
using Ref = T&;

template<typename T>
concept floating_point = std::is_floating_point_v<T>;

template<typename T>
concept C = std::is_integral_v<T> || (sizeof(T) > 1);

////////////////////////////////////////////////////////////////////////////////////////
//                                   WTF WARNING                                      //
////////////////////////////////////////////////////////////////////////////////////////
// disjunction
template<typename T, typename U>
concept C1 = std::is_integral_v<typename T::type> || std::is_integral_v<typename U::type>;
// atomic constraints
template<typename T, typename U>
concept C2 = bool(std::is_integral_v<typename T::type> || std::is_integral_v<typename U::type>);

// negative negative is not positive
// T has type, and it is an int
template<typename T>
concept C3 = std::is_integral_v<typename T::type>;
// T has type, and it is NOT an int
template<typename T>
concept C4 = !std::is_integral_v<typename T::type>;
// NOT(T has type, and it is an int)
template<typename T>
concept C5 = !C3<T>;

template <typename T>
concept R1 = requires(T a, T b) {
  // simple requirements
  a.test;
  T::static_test;
  a + b;
  // type requirements
  typename T::type;
  typename Empty<T>;
  typename Ref<T>;
  // compound requirements:
  // {expression} noexcept(optional) return-type-requirement(optional);
  // return-type-requirement: -> type-constraint
  {a.test};
  {a = std::move(b)} noexcept;
  {a + 1} -> std::same_as<int>; // equivalent to: require std::same_as<decltype(x+1), int>;
  // nested requirements
  // requires constraint-expression
  requires sizeof(T) > 1;
};

// require-clause
// require and enable_if
template <typename T>
std::enable_if_t<std::is_arithmetic_v<T>, T> foo_enable(T a, T b) {
  return a + b;
}

template <typename T>
requires std::is_arithmetic_v<T> T foo_require(T a, T b) {
  return a + b;
}

template<typename T>
auto bar_enable(T&& a) -> decltype(a.test) {
  return a.test;
}

template<typename T>
requires requires (T t){t.test;} int bar_require(T&& a) {
  return a.test;
}

int main() {
  static_assert(floating_point<float>);
  static_assert(C<int>);
  static_assert(C1<int, FooI>);
  static_assert(!C2<int, FooI>);
  static_assert(!C3<float>);
  static_assert(!C4<float>);
  static_assert(C5<float>);
  static_assert(R1<Test>);
  std::cout << "enable: " << foo_enable(1,2) << " require: " << foo_require(1,2) << std::endl;
  /* foo_enable(nullptr, nullptr); */
  /* foo_require(nullptr, nullptr); */
  std::cout << "enable: " << bar_enable(Test()) << " require: " << bar_require(Test()) << std::endl;
  /* bar_enable(1); */
  /* bar_require(1); */

}
