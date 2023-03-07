#include <type_traits>
#include <array>
#include <concepts>
#include <iostream>

// macro, general template and template with concept
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

template<typename... Args>
struct dump;

template<typename T>
T max_template(T x, T y) {
  return x > y ? x : y;
}

template<typename T>
concept Comparable = requires (T x, T y) {
  {x > y} -> std::same_as<bool>;
};

// equivalent implementation
// template<typename T>
// requires Comparable<T>
template<Comparable T>
T max_concept(T x, T y) {
  return x > y ? x : y;
}

// value eval
// template class
template<size_t N>
struct Fib {
  static constexpr int value = Fib<N-1>::value + Fib<N-2>::value;
};
template<>
struct Fib<0> { static constexpr int value = 0; };
template<>
struct Fib<1> { static constexpr int value = 1; };

// template variable
template<size_t N>
constexpr int fib = fib<N-2> + fib<N-1>;
template<> constexpr int fib<0> = 0;
template<> constexpr int fib<1> = 1;

// type eval
template<typename T, size_t I, size_t... Args>
struct Array {
  using type = std::array<typename Array<T, Args...>::type, I>;
};
template<typename T, size_t N>
// !!!!Array<T, N> instead of Array!!!!
struct Array<T, N> {
  using type = std::array<T,N>;
};

// TypeList
template <typename... Ts>
struct TypeList{
  template<typename... Args>
  using push = TypeList<Ts..., Args...>;
  static constexpr size_t size = sizeof...(Ts);

  struct IsTypeList{};
  using type = TypeList;

  // TRICKS HERE:
  // if template is DEFINED as folloing:
  // template<size_t N, typename IT, typename... ITs> struct IndexImpl{};
  // then TypeList<> causes compile error since IT is not provided in `TypeList<>`
  // Use template<size_t N, typename... ITs> as definition,
  // and template<size_t N, typename IT, typename... ITs> as specification.
  template<size_t N, typename... ITs>
  struct IndexImpl {};
  template<size_t N,
           typename IT,
           typename... ITs>
  requires (N<=sizeof...(ITs))
  struct IndexImpl<N, IT, ITs...> {
    using type = IndexImpl<N-1, ITs...>::type;
  };
  template<typename IT,
           typename... ITs>
  struct IndexImpl<0, IT, ITs...> {
    using type = IT;
  };

  template<size_t N>
  using at = IndexImpl<N, Ts...>::type;

};

template<typename In>
concept TL = requires(In in) {
  typename In::IsTypeList;
  typename In::type;
};

// Higher-order function

// declare Map with TypeList in template parameter got an compile error:
// error: type/value mismatch at argument 1 in template parameter list
// since compiler can not tell `TypeList<Args...>` is a type or value in
// specification: `Map<F, TypeList<Args...> >`
// template<template<typename> class F, TypeList In>
//                                      ~~~~~~~~
//                                      ^^^^^^^^ error here
template<template<typename> class F, TL In> // use TL or typename for In
struct Map;
template<template<typename> class F, typename... Args>
struct Map<F, TypeList<Args...> > : TypeList<typename F<Args>::type...> { };

template<template<typename> class F, TL in, TL Out = TypeList<> >
struct Filter : public Out {}; // end condition
template<template<typename> class F, typename T, typename... Args, TL Out>
struct Filter<F, TypeList<T, Args...>, Out > :
  std::conditional_t<F<T>::value,
                   // WTF is typename Out::template push<T> ?
                   // I don't know but Out::push<T> doesn't work.
                   // It is understandable that `push` is a template alias and
                   // need to use `Out::template push<T>` to represent push.
                   // But still WTF!
                   Filter<F, TypeList<Args...>, typename Out::template push<T> >,
                   Filter<F, TypeList<Args...>, Out> > {
}; // use std::condition to derive iteratively

int main() {
  int a{1};
  int b{2};
  /* std::cout << MAX(a++,b++) << ", " << a <<  ", " << b << std::endl; */
  std::cout << MAX(a, b) << ", " << max_template(a, b) <<  ", " << max_concept(a, b) << std::endl;

  static_assert(Fib<10>::value == 55);
  static_assert(fib<10> == 55);
  static_assert(std::is_same_v<Array<int,1,2>::type, std::array<std::array<int,2>,1>>);
  static_assert(std::is_same_v<TypeList<int, float, double>, TypeList<int>::push<float, double>>);
  static_assert(TypeList<int>::size == 1);
  static_assert(TypeList<int, bool, double>::push<double, double>::size == 5);
  static_assert(std::is_same_v<TypeList<int, float, double>::at<2>, double>);
  static_assert(std::is_same_v<TypeList<int>::at<0>, int>);
  static_assert(std::is_same_v<TypeList<int*, float*, double*>,
                               Map<std::add_pointer, TypeList<int, float, double>>::type>);
  /* dump<Map<std::add_pointer, TypeList<int, float, double>>::type>{}; */
  static_assert(std::is_same_v<TypeList<char, int>,
                               Filter<
                                      std::is_integral,
                                      TypeList<char, int, float, double>
                                      >::type
                              >);
  /* dump< Filter< */
  /*             std::is_integral, */
  /*             TypeList<char, int, float, double> */
  /*             >::type> {}; */
  // following code raise an error may due to compiler version.
  // template<typename T> */
  // using SizeLess4 = std::bool_constant<(sizeof(T) < 4)>; */
}
