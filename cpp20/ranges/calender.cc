#include <iostream>
#include <string>
#include <chrono>
#include <string>
#include <ranges>
#include <concepts>
#include <type_traits>
#include <vector>

namespace chrono = std::chrono;
namespace ranges = std::ranges;


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


int main() {
  std::vector X {
    std::vector{1, 2, 3, 4},
    std::vector{-1, 0, 1},
    std::vector{-2, 2, -3, 3},
  };

  print_range(X, ", ", 2);

  std::cout << Date(1992,10,13) << std::endl;
  print_range(dates_between(2022, 2023), ", ");

}
