#include <iostream>
#include <string>
#include <type_traits>
#include <concepts>
#include <algorithm>

template<size_t N>
struct FixedString {
  constexpr FixedString(const char(&s) [N]) {
    std::copy_n(s, N, str);
  }
  constexpr size_t getSize() const {
    return size - 1;
  }
  char str[N];
  constexpr static size_t size = N;
};

template <typename T>
consteval size_t strLength(T&& in) {
  return FixedString(in).getSize();
}

// equivalent implement with strLength
// operator ""_fs is required to convert auto param to FixString.
constexpr size_t getSize(auto in) {
  return in.getSize();
}

template<FixedString str>
constexpr decltype(str) operator""_fs () {return str;}

int main() {
  /* constexpr auto str = join(",", "one", "two", "three"); */
  static_assert(getSize("one"_fs) == 3);
  static_assert(strLength("one") == 3);
}
