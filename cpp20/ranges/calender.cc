#include <iostream>
#include <string>
#include <chrono>
#include <string>
#include <ranges>
#include <concepts>
#include <type_traits>
#include <vector>
#include <algorithm>
// #include <format>

namespace chrono = std::chrono;
namespace ranges = std::ranges;
namespace views = std::views;


void print_range(ranges::viewable_range auto&& range,
                 const std::string_view& delim = "",
                 size_t depth = 0,
                 bool last_cr = true) {
  std::cout << "[";
  bool first_token = false;
  for (auto&& v : range) {
    if (first_token) std::cout << delim;
    if constexpr (requires {print_range(v);}) {
      if (first_token) {
        std::cout << std::endl;
        for (auto d = 0; d < depth; ++d) std::cout << " ";
      }
      print_range(v, delim, depth+1, false);
    } else {
      std::cout << v;
    }
    first_token = true;
  }
  std::cout << "]";
  if (last_cr) std::cout << std::endl;
}



struct Date{
  using difference_type = std::ptrdiff_t;
  Date() = default;
  Date(uint16_t year, uint16_t month, uint16_t day) : days_(chrono::year(year) / chrono::month(month) / chrono::day(day)) {}

  bool operator==(const Date& rhs) const = default;
  Date& operator++() {++days_; return *this;}
  Date operator++(int) {Date tmp(*this); ++(*this); return tmp;}

  // interfaces
  uint16_t day() const {
    return static_cast<unsigned>(chrono::year_month_day(days_).day());
  }
  uint16_t month() const {
    return static_cast<unsigned>(chrono::year_month_day(days_).month());
  }
  uint16_t year() const {
    return static_cast<int>(chrono::year_month_day(days_).year());
  }
  friend std::ostream& operator<<(std::ostream& out, const Date& d) {
    out << d.year() << "-" << d.month() << "-" << d.day();
    return out;
  }

  const char* monthName() const {
    return MONTH_NAME[month()];
  }
  uint16_t dayOfWeek() const { // 得到当前日期星期数，值域[0,7)，0表示星期天
      return chrono::weekday(days_).c_encoding();
  }
  bool weekdayLessThan(const Date& rhs) const {
    return dayOfWeek() < rhs.dayOfWeek();
  }

private:
  static constexpr const char* MONTH_NAME[] = {
      "", "January", "February", "March",
      "April", "May", "June", "July",
      "August", "September", "October",
      "November", "December",
  };
  chrono::sys_days days_;
};

auto dates_between(uint16_t start, uint16_t stop) {
  return std::views::iota(Date(start, 1, 1), Date(stop, 1, 1));
}

template<ranges::input_range Rng, typename Func>
requires ranges::range<Rng>
struct group_by_view : public ranges::view_interface<group_by_view<Rng, Func>> {

  group_by_view() = default;
  group_by_view(Rng r, Func f) : r_(std::move(r)), f_(std::move(f)) {}

  struct group_iterator {
    using difference_type = std::ptrdiff_t;
    using value_type = ranges::subrange<ranges::iterator_t<Rng>>;

    group_iterator& operator++() {
      cur = next_cur;
      if (cur != last) {
        next_cur = ranges::find_if_not(ranges::next(cur), last, [&](auto&& elem){return f(*cur, elem);});
      }
      return *this;
    }

    group_iterator operator++(int) {
      group_iterator tmp(*this);
      ++(*this);
      return tmp;
    }

    bool operator==(const group_iterator&) const = default;
    bool operator==(std::default_sentinel_t) const { return cur == last; }
    value_type operator*() const { return {cur, next_cur}; }
    Func f;
    ranges::iterator_t<Rng> cur{};
    ranges::iterator_t<Rng> next_cur{};
    ranges::sentinel_t<Rng> last{};
  };

  group_iterator begin() {
    auto beg = ranges::begin(r_);
    auto end = ranges::end(r_);
    return {f_,
            beg,
            ranges::find_if_not(ranges::next(beg), end, [&](auto&& elem){return f_(*beg, elem);}),
            end};
  }
  std::default_sentinel_t end() { return {}; }

private:
  Rng r_;
  Func f_;
};

/* // !&%$#$@*&)*&$^$ */
struct GroupWrapper : std::views::__adaptor::_RangeAdaptor<GroupWrapper> {
  // XXX: constexpr auto: not sure why must declare return type as auto. Or a compile error is raised.
  template<typename Rng, typename Func>
  constexpr auto operator()(Rng&& r, Func&& f) const {
    return group_by_view{std::forward<Rng>(r), std::forward<Func>(f)};
  }
  static constexpr int _S_arity = 2;
  using std::views::__adaptor::_RangeAdaptor<GroupWrapper>::operator();
};

inline constexpr GroupWrapper group_by;

auto by_month() {
  return group_by([](Date a, Date b) { return a.month() == b.month();});
}

auto by_week() {
  return group_by([](Date a, Date b) { return a.weekdayLessThan(b);  });
}


int main() {
  std::vector X {
    std::vector{1, 2, 3, 4},
    std::vector{-1, 0, 1},
    std::vector{-2, 2, -3, 3},
  };

  print_range(X, ", ", 2);

  std::cout << Date(1992,10,13) << std::endl;
  print_range(dates_between(2022, 2023), ", ");

  print_range(dates_between(2022, 2023) | by_month(), ", ");

  /* static_assert(ranges::range<group_by_view<decltype(dates_between(2022, 2023)), decltype([](Date a, Date b) { return a.month() == b.month();})>>); */

}
