#pragma once
#include <vector>
#include <type_traits>
#include <array>
#include <concepts>
#include <iostream>

template <typename T>
void print_vector(const std::vector<T>& input) {
  for (auto& elem : input) {
    std::cout << elem << " ";
  }
  std::cout << std::endl;
}

template<typename T>
int partition(std::vector<T>& input, int low, int high) {
  int base = low;
  T pivot = input[low];

  while (low < high) {
    while((low < high) && (input[high] > pivot)) --high;
    input[low] = input[high];
    while((low < high) && (input[low] <= pivot)) ++low;
    input[high] = input[low];
  }
  input[low] = pivot;
  return low;
}

template<typename T>
void quick_sort_impl(std::vector<T>& input, int low, int high) {
  if (high - low < 2) return;
  auto rank = partition(input, low, high - 1);
  quick_sort_impl(input, low, rank);
  quick_sort_impl(input, rank + 1, high);
}

template<typename T>
void quick_sort(std::vector<T>& input) {
  quick_sort_impl(input, 0, input.size());
}

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

template<template<typename, typename> class F, TL in, typename Init >
struct Fold {
  using type = Init;
}; // end condition
template<template<typename, typename> class F, typename T, typename... Args, typename Acc>
struct Fold<F, TypeList<T, Args...>, Acc > : Fold<F, TypeList<Args...>, typename F<Acc, T>::type > {};

// algorithm
// Concat
template<TL... Tls>
struct Concat;
template<TL In, TL... Tls>
struct Concat<In, Tls...> : Concat<In, typename Concat<Tls...>::type>::type {};
template<typename... Args1, typename... Args2>
struct Concat<TypeList<Args1...>, TypeList<Args2...>> {
  using type = TypeList<Args1..., Args2...>;
};
template<>
struct Concat<> : TypeList<> {};

// Elem(find if exist)
template<TL In, typename T>
struct Elem {
  template<typename E, size_t V=0>
  struct AccImpl {
    static constexpr size_t value = V;
    using type = AccImpl;
  };
  // Found and return if exists
  template <typename Acc, typename E>
  using FindE = std::conditional_t<Acc::value, Acc, std::is_same<E, T>>;
  using Found = Fold<FindE, In, std::false_type>;
  // Found and return all occurrence
  template<typename Acc, typename C>
  using ElemAcc = AccImpl<T, Acc::value + std::is_same_v<T, C>>;
  static constexpr size_t value = Fold<ElemAcc, In, AccImpl<T, 0> >::type::value;
};

// Unique
template<TL In>
struct Unique {
  template <TL Acc, typename C>
  using Append = std::conditional<Elem<Acc, C>::Found::type::value,
                                  Acc,
                                  typename Acc::template push<C>>;
  using type = Fold<Append, In, TypeList<>>::type;
};

// Partition
// template<TL In, template<typename> typename Cond>
//                                    ^^^^^^^^ is required!
// see https://en.cppreference.com/w/cpp/language/template_parameters
// template template parameter
template<template<typename> typename Cond, TL In>
struct Partition {
  template<typename T>
  using NotCond = std::negation<Cond<T>>;
  using Satisfy = Filter<Cond, In>::type;
  using UnSatisfy = Filter<NotCond, In>::type;
};

// Sort
template<template<typename, typename> typename Cmp, TL In>
struct Sort : TypeList<> {};
template<template<typename, typename> typename Cmp, typename T, typename... Args>
struct Sort<Cmp, TypeList<T, Args...>> {
  template<typename E>
  using Compare = Cmp<T, E>;
  using debug = Partition<Compare, TypeList<Args...>>::UnSatisfy;
  using LowRank = Sort<Cmp, typename Partition<Compare, TypeList<Args...>>::Satisfy>::type;
  using HighRank = Sort<Cmp, typename Partition<Compare, TypeList<Args...>>::UnSatisfy>::type;
  using type = Concat<LowRank, TypeList<T>, HighRank>::type;
};

// CrossProduct
template<TL A, TL B, template<typename, typename> typename Pair>
struct CrossProduct {
  template<TL OuterResult, typename ElemA>
  struct OuterAppend {
    template<TL InnerResult, typename ElemB>
    using InnerAppend = typename InnerResult::template push<Pair<ElemA, ElemB>>;
    using type = Fold<InnerAppend, B, OuterResult>::type;

  };
  using type = Fold<OuterAppend, A, TypeList<>>::type;
};

// helpers
template<template<typename> class F, TL In>
using Map_t = Map<F, In>::type;
template<template<typename> class F, TL in, TL Out = TypeList<> >
using Filter_t = Filter<F, in, Out>::type;
template<template<typename, typename> class F, TL in, typename Init >
using Fold_t = Fold<F, in, Init>::type;
template<TL... Tls>
using Concat_t = Concat<Tls...>::type;
template<TL In, typename T>
constexpr bool Elem_v = Elem<In, T>::value;
template<TL In>
using Unique_t = Unique<In>::type;
template<template<typename, typename> typename Cmp, TL In>
using Sort_t = Sort<Cmp, In>::type;

// test templates
template<typename T>
using SizeLess4 = std::bool_constant<(sizeof(T) < 4)>;

template<typename Acc, typename T>
using TypeSizeAcc = std::integral_constant<size_t, Acc::value + sizeof(T)>;

template<typename T1, typename T2>
using CompareSize = std::bool_constant<(sizeof(T1) > sizeof(T2))>;
/* template<typename Acc, typename C> */
/* using ElemAcc = std::integral_constant<float, Acc::value + std::is_same_v<float, C>>; */
