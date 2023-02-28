#include <iostream>
#include <type_traits>

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
    std::cout << "WithOptimization Nonempty Classsize: " << sizeof(WithOptimization<T1, T2, false>) << std::endl;
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

int main() {
  // fibonacci<10>== 55 cause a compile error: error: parse error in template argument list.
  // A SPACE is needed around ==.
  static_assert(fibonacci<10> == 55);

  std::cout << golden_ratio<20> << std::endl;
  std::cout << golden_ratio<60> << std::endl;

  using Two = integral_constant<int, 2>;
  using Four = integral_constant<int, 4>;

  static_assert(Two::value * Two::value == Four::value);
  std::cout << "PASS" << std::endl;

  // Empty class optimization
  Base()();
  Children()();
  Container()();
  NoOptimization<Base, int>()();
  WithOptimization<Base, int>()();
  NoUniqueAddress<Base>()();
  NoUniqueAddress<double>()();
}
