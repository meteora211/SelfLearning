#include <ranges>
#include <iostream>
#include <type_traits>
#include <concepts>
#include <vector>
#include <string>
#include <string_view>

namespace ranges = std::ranges;

namespace ns {
  struct Foo {};
  void swap (Foo&, Foo&) {
    std::cout << "custom ns::swap called." << std::endl;
  }
}

template<ranges::borrowed_range R>
void f(R&&) { }

int main() {
  ns::Foo a, b;
  std::cout << "call std::swap" << std::endl;
  std::swap(a,b);
  std::cout << "call ADL swap" << std::endl;
  swap(a,b);
  // XXX: ranges::swap still look up custom ns::swap
  std::cout << "call ranges::swap" << std::endl;
  ranges::swap(a,b);

  // Ranges concept:
  // range : template<typename T> concept range = requires(T& t) {ranges::begin(t); ranges::end(t);};
  // borrowed_range: ...
  // sized_range: template<typename T> concept range = range<T> && requires(T& t) {ranges::size(t);};
  // view: ...
  // ...
  static_assert(ranges::range<std::vector<int>>);
  // compile error: rvalue used
  // f(vector<int>{1,2,3});
  std::vector<int> v{1,2,3};
  f(v); // lvalue ref used in f
  f(std::string_view{"123"}); // it works because enable_borrowed_range<string_view> is true.
  static_assert(ranges::sized_range<std::vector<int>>);
  static_assert(!ranges::view<std::vector<int>>);
  static_assert(ranges::view<std::string_view>);

}
