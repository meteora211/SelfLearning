#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>
#include <iterator>

using namespace std::placeholders;

template <typename T>
struct plus {
  T operator()(T x, T y) {
    return x + y;
  }
};

int main() {
  // bind
  std::cout << plus<int>{}(2,3) << std::endl;
  auto plus5 = std::bind(plus<int>{}, 5, _1);
  std::cout << plus5(3) << std::endl;

  std::vector<int> nums {1,2,3,4,5,6,7,8,9};
  std::copy_if(nums.begin(), nums.end(), std::ostream_iterator<int>(std::cout , ","), std::bind(std::greater<int>{}, _1, 4));

  // lambda. use c++17 to compile
  constexpr auto add = [](auto x, auto y) { return x + y; };
  constexpr auto add5 = [add](auto x){return add(x,5);};
  std::cout << add5(3) << std::endl;
  std::copy_if(nums.begin(), nums.end(), std::ostream_iterator<int>(std::cout , ","), [](int x){ return x > 4; });

}
