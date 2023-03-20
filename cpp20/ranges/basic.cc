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

template <typename T>
class VectorView : public ranges::view_interface<VectorView<T>> {
public:
  VectorView() = default;
  ~VectorView() = default;
  // XXX: use VectorView(Vector<T> v) cause print mess since iterator is point to a temp copy.
  VectorView(const std::vector<T>& v) : begin_(v.cbegin()), end_(v.cend()) {}
  auto begin() const {return begin_;}
  auto end() const {return end_;}

private:
  typename std::vector<T>::const_iterator begin_{}, end_{};
};

int main() {
  ns::Foo a, b;
  std::cout << "call std::swap" << std::endl;
  std::swap(a,b);
  std::cout << "call ADL swap" << std::endl;
  swap(a,b);
  // XXX: ranges::swap still look up custom ns::swap
  std::cout << "call ranges::swap" << std::endl;
  ranges::swap(a,b);

  std::vector<int> v{1,2,3};
  int arr[] = {-1, -2, -3};
  // Ranges Access:
  std::cout << "ranges on vector and array." << std::endl;
  auto vb = ranges::begin(v);
  // XXX: arr is an array, ranges::begin returns arr + 0;
  auto ab = ranges::begin(arr);
  std::cout << "begin(): " << *vb << " " << *arr << std::endl;
  std::cout << "size(): " << ranges::size(v) << " " << ranges::size(arr) << std::endl;
  std::cout << "data(): " << ranges::data(v) << " " << ranges::data(arr) << std::endl;

  // Ranges Concepts:
  // range : template<typename T> concept range = requires(T& t) {ranges::begin(t); ranges::end(t);};
  // borrowed_range: ...
  // sized_range: template<typename T> concept range = range<T> && requires(T& t) {ranges::size(t);};
  // view: ...
  // ...
  static_assert(ranges::range<std::vector<int>>);
  static_assert(ranges::range<decltype(arr)>);
  // compile error: rvalue used
  // f(vector<int>{1,2,3});
  f(v); // lvalue ref used in f
  f(std::string_view{"123"}); // it works because enable_borrowed_range<string_view> is true.
  static_assert(ranges::sized_range<std::vector<int>>);
  static_assert(!ranges::view<std::vector<int>>);
  static_assert(ranges::view<std::string_view>);

  static_assert(ranges::view<VectorView<int>>);

  // Ranges View
  VectorView vec_view{v};
  for (auto i : vec_view) {
    std::cout << i  << " ";
  }
  std::cout << std::endl;

  //   subrange
  auto sr = ranges::subrange(v.begin(), v.end() - 1);
  for (auto i : sr) {
    std::cout << i  << " ";
  }
  std::cout << std::endl;

  //   ref_view
  auto rv = ranges::ref_view(v);
  for (auto i : rv) {
    std::cout << i  << " ";
  }
  std::cout << std::endl;

  // Ranges Factories
  auto ev = ranges::empty_view<int>{};
  static_assert(ev.empty());

  auto sv = ranges::single_view{6};
  static_assert(ranges::size(sv)==1);

  // iota_view,  itoa func
  // For int-like class in iota_view, See:
  // https://stackoverflow.com/questions/74695190/how-to-use-iota-view-for-custom-intlike-types
  for (auto i: ranges::iota_view(0, 5)) {
    std::cout << i  << " ";
  }
  std::cout << std::endl;

  // Adaptors
  std::vector ints{1,2,3,4,5,6,7,8,9};
  auto even = [](int i){ return i % 2 == 0; };
  auto square = [](int i) {return i * i;};

  // https://en.cppreference.com/w/cpp/ranges#Range_adaptor_closure_objects
  //  viewable_range/range(c++20/23) -> adaptor -> view(c++20)
  //  given C: range adaptor closure object and R: viewable_range/range(c++20/23)
  //  C(R) == R | C
  // So following condition is equivalent to
  // for (auto i : transform(filter(ints, even), square)) {
  for (auto i : ints | std::views::filter(even) | std::views::transform(square) ) {
    std::cout << i << " ";
  }
  std::cout << std::endl;

  // The viewable_range concept is a refinement of range that describes a range that can be converted into a view through views::all.
  // for (auto i : ints | std::views::take(3)) {
  // equivalents to:
  for (auto i : std::views::all(ints) | std::views::take(3)) {
    std::cout << i << " ";
  }
  std::cout << std::endl;

  std::vector<std::vector<std::string_view>> vsv {{"hello"}, {" ", "world"}, {"!"}};
  for (auto i : vsv | std::views::join) {
    std::cout << i;
  }
  std::cout << std::endl;

  auto sum_view = std::views::iota(1)
             | std::views::take_while([](int n){return n <= 1000;})
             | std::views::filter([](int n){return (n % 2) != 0;})
             | std::views::transform([](int n){return n * n;});
  int sum = 0;
  for (auto s : sum_view) {
    // XXX: sum_view is lazy computed during iteration.
    sum += s;
  }
  std::cout << sum << std::endl;

}
