#include "template_traits.h"
#include <vector>
#include <type_traits>
#include <array>
#include <concepts>
#include <iostream>

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
  static_assert(std::is_same_v<TypeList<char>,
                               Filter<
                                      SizeLess4,
                                      TypeList<char, int, float, double>
                                      >::type
                              >);
  static_assert(Fold<TypeSizeAcc, TypeList<int, double>, std::integral_constant<size_t, 0>>::type::value == 12);
  static_assert(std::is_same_v<Concat<TypeList<int, float, double>, TypeList<int, float, double>>::type,
                               TypeList<int, float, double, int, float, double>>);
  static_assert(std::is_same_v<Concat<TypeList<int, float>, TypeList<char, int*>, TypeList<float, double>>::type,
                               TypeList<int, float, char, int*, float, double>>);
  /* dump<Elem<TypeList<int, float, char, int*, float, double>, float>::ElemAcc>{}; */
  /* static_assert(Fold<ElemAcc, TypeList<int, double>, std::integral_constant<float, 0>>::type::value == 0); */
  static_assert(Elem<TypeList<int, float, char, int*, float, double>, float>::value == 2);
  static_assert(std::is_same_v<Unique<TypeList<int, float, char, int*, float, double>>::type,
                               TypeList<int, float, char, int*, double>>);
  static_assert(std::is_same_v<TypeList<char>,
                               Partition<SizeLess4,
                                         TypeList<char, int, float, double>>::Satisfy
                              >);
  static_assert(std::is_same_v<TypeList<int, float, double>,
                               Partition<SizeLess4,
                                         TypeList<int, float, double>>::UnSatisfy
                              >);
  static_assert(std::is_same_v<Sort<CompareSize, TypeList<int, float, char, int64_t, double, long double>>::type,
                               TypeList<char, int, float, int64_t, double, long double>>);
  /* dump<Sort<CompareSize, TypeList<int, float, char, int64_t, double, long double>>::type>{}; */

  std::vector<int> vec{1,3,4,7,2,6,3,6,2,1,5,8,45,68,3,4,12};
  quick_sort(vec);
  print_vector(vec);
}
