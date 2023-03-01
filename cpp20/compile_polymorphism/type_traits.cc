#include <vector>
#include <iostream>
#include <type_traits>
#include <tuple>
#include <limits>
#include <cmath>
#include <string>

// dump helper
template<typename, typename...>
struct dump;

template<size_t v>
constexpr size_t fibonacci = (fibonacci<v - 1> + fibonacci<v - 2>);

template<>
constexpr size_t fibonacci<0> = 0;

template<>
constexpr size_t fibonacci<1> = 1;

template<size_t v>
constexpr double golden_ratio = fibonacci<v> * 1.0  / fibonacci<v - 1>;


template<typename T, T v>
struct integral_constant {
  using type = integral_constant;
  using value_type = T;
  // equivalent: enum {value = v};
  static constexpr T value = v;
};


// Empty class Optimization
struct Base {
  void operator()() {
    std::cout << "Empty Base size:" << sizeof(Base) << std::endl;
  }
};

struct Children : public Base {
  int i;
  void operator()() {
    std::cout << "Children size:" << sizeof(Children) << std::endl;
  }
};

struct Container {
  Base b;
  int i;
  void operator()() {
    std::cout << "Container size:" << sizeof(Container) << std::endl;
  }
};

template<typename T1, typename T2>
struct NoOptimization {
  T1 t1;
  T2 t2;
  void operator()() {
    std::cout << "NoOptimization size: " << sizeof(NoOptimization<T1, T2>) << std::endl;
  }
};

template<typename T1, typename T2, bool = std::is_empty_v<T1> && !std::is_final_v<T1>>
struct WithOptimization final : private T1 {
  T2 t2;
  void operator()() {
    std::cout << "WithOptimization Empty Class size: " << sizeof(WithOptimization<T1, T2>) << std::endl;
  }
};

template<typename T1, typename T2>
struct WithOptimization<T1, T2, false> final {
  T1 t1;
  T2 t2;
  void operator()() {
    std::cout << "WithOptimization Nonempty Class size: " << sizeof(WithOptimization<T1, T2, false>) << std::endl;
  }
};

template<typename T>
struct NoUniqueAddress {
  [[no_unique_address]] T base;
  int i;
  void operator()() {
    std::cout << "NoUniqueAddress size: " << sizeof(NoUniqueAddress<T>) << std::endl;
  }
};

// some traits excise
template<typename T>
struct array_size;

template<typename T, size_t N>
struct array_size<T[N]> {
  using type = T;
  static constexpr int value = N;
};

template<typename F>
struct function_trait;

template<typename Return, typename...Args>
struct function_trait<Return(Args...)> {
  using return_type = Return;
  using args_type = std::tuple<Args...>;
  // std::tuple_element_t<N, args_type>: returns a TYPE
  // Then define arg as a type alias: https://en.cppreference.com/w/cpp/language/type_alias
  template<size_t N> using arg = std::tuple_element_t<N, args_type>;
};

// enable_if
template<bool,  typename = void>
struct enable_if_impl {};
template<typename T>
struct enable_if_impl<true, T> {
  using type = T;
};
template<bool B, typename T = void> using enable_if_impl_t = typename enable_if_impl<B, T>::type;

// use following template definition for 2 numEq cause a compile error:
// error: redefinition of ‘template<class T, class> bool numEq(T, T)’
// see example in https://en.cppreference.com/w/cpp/types/enable_if
// template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
template<typename T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
bool numEq(T lhs, T rhs) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  return lhs == rhs;
}
template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
bool numEq(T lhs, T rhs) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  return std::fabs(lhs - rhs) < std::numeric_limits<T>::epsilon();
}

// Tag dispatching
template <typename T>
bool numEqTDImpl(T lhs, T rhs, std::false_type) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  return lhs == rhs;
}

template <typename T>
bool numEqTDImpl(T lhs, T rhs, std::true_type) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  return std::fabs(lhs - rhs) < std::numeric_limits<T>::epsilon();
}

template <typename T>
auto numEqTD(T lhs, T rhs) -> std::enable_if_t<std::is_arithmetic_v<T>, bool> {
  return numEqTDImpl(lhs, rhs, std::is_floating_point<T>{});
}

// if constexpr
template <typename T>
auto numEqIf(T lhs, T rhs) -> std::enable_if_t<std::is_arithmetic_v<T>, bool> {
  if constexpr (std::is_floating_point_v<T>) {
    return std::fabs(lhs - rhs) < std::numeric_limits<T>::epsilon();
  } else {
    return lhs == rhs;
  }
}

// void_t
template<typename T, typename = void>
struct HasTypeMember : public std::false_type {};

template<typename T>
struct HasTypeMember<T, std::void_t<typename T::type>> : public std::true_type {};

template<typename T, typename = void>
struct HasInit : public std::false_type {};

template<typename T>
struct HasInit<T, std::void_t<decltype(std::declval<T>().Init())>> : public std::true_type {};

class Foo {
public:
  void Init(){}
};


int main() {
  // fibonacci<10>== 55 cause a compile error: error: parse error in template argument list.
  // A SPACE is needed around ==.
  static_assert(fibonacci<10> == 55);

  std::cout << golden_ratio<20> << std::endl;
  std::cout << golden_ratio<60> << std::endl;

  using Two = integral_constant<int, 2>;
  using Four = integral_constant<int, 4>;

  static_assert(Two::value * Two::value == Four::value);
  // static member variable doesn't counted in class size
  std::cout << "size of std::true_type: " << sizeof(std::true_type) << std::endl;
  std::cout << "size of Two: " << sizeof(Two) << std::endl;

  // Empty class optimization
  Base()();
  Children()();
  Container()();
  NoOptimization<Base, int>()();
  WithOptimization<Base, int>()();
  NoUniqueAddress<Base>()();
  NoUniqueAddress<double>()();

  // traits
  static_assert(array_size<int[5]>::value == 5);
  static_assert(std::is_same_v<array_size<int[5]>::type, int>);

  using func = void(int, float, std::vector<char>);
  static_assert(std::is_same_v<function_trait<func>::return_type, void>);
  static_assert(std::is_same_v<function_trait<func>::arg<0>, int>);

  static_assert(std::is_same_v<enable_if_impl_t<std::is_integral_v<int>, int >, int>);
  std::cout << "numEq(1,2): " << numEq(1, 2) << std::endl;
  std::cout << "numEq(1.0,2.0): " << numEq(1.0, 2.0) << std::endl;
  std::cout << "numEqTD(1,2): " << numEqTD(1, 2) << std::endl;
  std::cout << "numEqTD(1.0,2.0): " << numEqTD(1.0, 2.0) << std::endl;
  std::cout << "numEqIf(1,2): " << numEqIf(1, 2) << std::endl;
  std::cout << "numEqIf(1.0,2.0): " << numEqIf(1.0, 2.0) << std::endl;
  // following usage got an expected error. Because tail return std::enable_if_t<std::is_arithmetic_v<T>, bool>
  // has no type info when std::is_arithmetic_v<T> is false.
  // error: no matching function for call to ‘numEqTD(const char [5], const char [5])’
  // std::cout << "numEqTD(string, string): " << numEqTD("test", "test") << std::endl;
  /* dump<enable_if_impl_t<std::is_integral_v<int> >>{}; */

  static_assert(!HasTypeMember<int>::value);
  static_assert(HasTypeMember<std::true_type>::value);

  static_assert(!HasInit<int>::value);
  static_assert(HasInit<Foo>::value);

  std::cout << "PASS" << std::endl;
}
