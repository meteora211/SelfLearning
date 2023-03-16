#include <iostream>
#include <type_traits>
#include <numbers>
#include <array>
#include <cmath>

// constinit, consteval, constexpr
// See more in https://www.cppstories.com/2022/const-options-cpp20/

consteval int sum_eval(int v1, int v2) {
  return v1 + v2;
}

constexpr int sum_expr(int v1, int v2) {
  return v1 + v2;
}

constinit int global = sum_eval(10, 10);
// Compile error here. constinit can not be used in constant expression
// ‘constinit’ variable ‘another’ does not have a constant initializer
// constinit int another = global;


// fold expressions
// (pack op ... [op init])
// ([init op] ... pack op)
template <typename... Args>
auto sum(Args... args) {
  return (args + ...);
//        ^^  ^^
//    ( pack  op ... )
}

template <typename... Args>
auto sum_with_zero(Args... args) {
  return (0 + ... + args);
//        ^^      ^^  ^^
//    (  init ... op  pack )
}

template <typename... Args>
auto lfold(Args... args) {
  return (10 - ... - args);
//        ^^      ^^  ^^
//    (  init ... op  pack )
}

template <typename... Args>
auto rfold(Args... args) {
  return (args - ... - 10);
//        ^^   ^^      ^^
//    (  pack  op ...  init )
}

struct Shape{
  virtual ~Shape() = default;
  virtual double getArea() const = 0;
};

struct Circle : public Shape{
  constexpr Circle(double r) : r_(r) {}
  constexpr double getArea() const override {
    return std::numbers::pi * r_ * r_;
  }
  constexpr ~Circle() {};
private:
  double r_;
};

struct Rectangle : public Shape{
  constexpr Rectangle(double m, double n) : m_(m), n_(n) {}
  constexpr double getArea() const override {
    return m_ * n_;
  }
  // XXX: constexpr Rectangle r = Rectangle(2,3) cause compiler error
  // which is A BUG in gcc: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=93413
  // ~Rectangle() = default;
  constexpr ~Rectangle() {};
private:
  double m_;
  double n_;
};

constexpr double power(double b, const int x) {
  if (std::is_constant_evaluated()) {
    // XXX: issue here. how to convert glvalue to constant expression?
    if constexpr (helper<x>::value > 0) {
      return b * power(b, x-1);
    } else {
      return 1;
    }
  } else {
    std::cout << "runtime call" << std::endl;
    return std::pow(b, x);
  }
}

consteval bool areaEqual(const Shape* shape, double area) {
  return shape->getArea() == area;
}

consteval bool areaEqual(const Shape& shape, double area) {
  return shape.getArea() == area;
}

int main() {
  constexpr auto val = 100;
  static_assert(sum_eval(val, val) == 200);
  static_assert(sum_expr(val, val) == 2* val);

  int a = 10;
  int b = sum_expr(a, 10);
  // Compile error: error: the value of ‘a’ is not usable in a constant expression
  // int c = sum_eval(a, 10);

  std::cout << sum(1,2,3,4) << std::endl;
  std::cout << sum_with_zero() << std::endl;
  std::cout << lfold(1,2,3,4) << std::endl; // ((((10 - 1) - 2) - 3) - 4) = 0
  std::cout << rfold(1,2,3,4) << std::endl; // (1 - (2 - (3 - (4 - 10)))) = 8

  // compiler error: error: the value of ‘<anonymous>’ is not usable in a constant expression
  // Maybe compiler is not updated. No clue yet
  // constexpr auto* shape1 = new Circle(3);
  // constexpr Shape* shape2 = new Rectangle(3, 2);
  // delete shape1;
  // delete shape2;

  constexpr auto shape1 = Circle(3);
  constexpr auto shape2 = Rectangle(3, 2);
  static_assert(areaEqual(shape1, 9*std::numbers::pi));
  static_assert(areaEqual(shape2, 6));

}
