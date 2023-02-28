#include <iostream>
#include <memory>
#include <variant>

#define PI 3.141592657

class Shape {
public:
  Shape() = default;
  virtual ~Shape() = default;
  virtual double getArea() const = 0;
  virtual double getPerimeter() const = 0;
};

class Rectangle : public Shape {
public:
  Rectangle() = delete;
   Rectangle(double m, double n) : m_(m), n_(n) {}
  ~Rectangle() = default;
  virtual double getArea() const final {
    return m_ * n_;
  }
  virtual double getPerimeter() const final {
    return 2 * (m_ + n_);
  }
private:
  double m_;
  double n_;
};

class Circle : public Shape {
public:
  Circle() = delete;
  explicit Circle(double r) : r_(r) {}
  ~Circle() = default;
  virtual double getArea() const final {
    return r_ * r_ * PI;
  }
  virtual double getPerimeter() const final {
    return 2 * PI * r_;
  }
private:
  double r_;
};

struct CircleV {double r;};
double getAreaV(const CircleV& c) {
  return c.r * c.r * PI;
}
double getPerimeterV(const CircleV& c) {
    return 2 * PI * c.r;
}

struct RectangleV {double m; double n;};
double getAreaV(const RectangleV& r) {
  return r.m * r.n;
}
double getPerimeterV(const RectangleV& r) {
  return 2 * (r.m * r.n);
}

using ShapeV = std::variant<CircleV, RectangleV>;

double getAreaV(const ShapeV& s) {
  return std::visit([](const auto& data){ return getAreaV(data); }, s);
}
double getPerimeterV(const ShapeV& s) {
  return std::visit([](const auto& data){ return getPerimeterV(data); }, s);
}

int main() {
  // polymorphism with virtual function
  std::shared_ptr<Shape> shape1 = std::make_shared<Circle>(3.0);
  std::shared_ptr<Shape> shape2 = std::make_shared<Rectangle>(3.0, 4.0);

  std::cout << shape1->getArea() << ", " << shape1->getPerimeter() << std::endl;
  std::cout << shape2->getArea() << ", " << shape2->getPerimeter() << std::endl;

  // polymorphism with variant
  ShapeV shape3 = CircleV{3};
  ShapeV shape4 = RectangleV{3,4};

  std::cout << getAreaV(shape3) << ", " << getPerimeterV(shape3) << std::endl;
  std::cout << getAreaV(shape4) << ", " << getPerimeterV(shape4) << std::endl;
}
