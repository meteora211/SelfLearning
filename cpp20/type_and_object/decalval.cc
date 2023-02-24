#include <utility>
#include <iostream>
#include <type_traits>

struct Default
{
  Default() {
    std::cout << "Default ctor" << std::endl;
  }

  int foo() const { return 1; }
};

struct NonDefault
{
  NonDefault() = delete;
  int foo() const { return 1; }
};

template<typename T>
constexpr bool always_false = false;

template<typename T>
typename std::add_rvalue_reference<T>::type mydeclval() noexcept {
  static_assert(always_false<T>, "declval not allowed in an evaluated context.");
}

int main()
{
  decltype(Default().foo()) n1 = 1;                   // type of n1 is int
  // decltype(NonDefault().foo()) n2 = n1;               // error: no default constructor
  decltype(std::declval<NonDefault>().foo()) n2 = n1; // type of n2 is int

  decltype(mydeclval<Default>().foo()) n3 = n1; // type of n2 is int
  // mydeclval<Default>().foo(); // error

  std::cout << "n1 = " << n1 << '\n' << "n2 = " << n2 << '\n' << "n3 = " << n3 << '\n';
}
