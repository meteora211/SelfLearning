#include <iostream>

template<typename Derived>
class Base {
public:
  ~Base() = default;
  void print() {
    static_cast<Derived&>(*this).printImpl();
  }

protected:
  Base() = default;
};

class HelloPrinter : public Base<HelloPrinter> {
public:
  HelloPrinter() = default;
  ~HelloPrinter() = default;

  void printImpl() {
    std::cout << "HelloPrinter" << std::endl;
  }
};

class WorldPrinter : public Base<WorldPrinter> {
public:
  WorldPrinter() = default;
  ~WorldPrinter() = default;

  void printImpl() {
    std::cout << "WorldPrinter" << std::endl;
  }
};

template <typename T>
void print(Base<T>& b) {
  b.print();
}

int main() {
  auto hp = HelloPrinter();
  auto wp = WorldPrinter();

  print(hp);
  print(wp);
}
