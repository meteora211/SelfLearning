#include <iostream>

class CustomInt {
public:
  static constexpr int Int = 5;
};

template<CustomInt I>
constexpr int foo() {
  return I.Int + 5;
}

int main() {
  CustomInt i;
  static_assert(foo<i>() == (i.Int + 5));
}
