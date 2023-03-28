#include <iostream>
#include <string>
#include <chrono>
#include <string>
#include <ranges>
#include <concepts>
#include <type_traits>
#include <vector>
#include <algorithm>

namespace ranges = std::ranges;
namespace views = std::views;

template<std::ranges::viewable_range Rng, typename Arg>
struct MulView{
  MulView() = default;
  MulView(Rng r, Arg arg) : r_(std::move(r)), arg_(std::move(arg)) {}

  struct MulIter {
    // XXX: initializer list compile error with following ctor
    /* MulIter() = default; */

    using difference_type = std::ptrdiff_t;

    decltype(auto) operator*() {
      return *cur * a;
    }
    MulIter& operator++() {
      ++cur;
      return *this;
    }
    MulIter operator++(int) {
      MulIter tmp(*this);
      ++cur;
      return tmp;
    }
    bool operator==(std::default_sentinel_t) const {
      return cur == last;
    }
    bool operator==(const MulIter& rhs) const {
      return cur == rhs.cur;
    }

    Arg a;
    ranges::iterator_t<Rng> cur{};
    ranges::iterator_t<Rng> last{};
  };

  MulIter begin() {
    auto beg = ranges::begin(r_);
    auto end = ranges::end(r_);
    return {arg_, beg, end};
  }
  std::default_sentinel_t end() { return {}; }

private:
  Rng r_;
  Arg arg_;
};

struct Mul : std::views::__adaptor::_RangeAdaptor<Mul> {
  template<std::ranges::viewable_range Rng, typename Arg>
  constexpr auto operator()(Rng&& range, Arg&& arg) const {
    return MulView{std::forward<Rng>(range), std::forward<Arg>(arg)};
  }

  static constexpr int _S_arity = 2;
  using std::views::__adaptor::_RangeAdaptor<Mul>::operator();
};


int main() {
  Mul mul;
  auto custom_view = views::iota(1, 100) | mul(2);

  for (auto&& v : custom_view) {
    std::cout << v << " ";
  }
  std::cout << std::endl;
}
