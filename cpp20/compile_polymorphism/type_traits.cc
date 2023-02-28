#include <vector>
#include <iostream>
#include <type_traits>
#include <tuple>

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

int main() {
  // fibonacci<10>== 55 cause a compile error: error: parse error in template argument list.
  // A SPACE is needed around ==.
  static_assert(fibonacci<10> == 55);

  std::cout << golden_ratio<20> << std::endl;
  std::cout << golden_ratio<60> << std::endl;

  using Two = integral_constant<int, 2>;
  using Four = integral_constant<int, 4>;

  static_assert(Two::value * Two::value == Four::value);

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

  std::cout << "PASS" << std::endl;
}
