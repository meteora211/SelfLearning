#include <iostream>
#include <thread>

void hello() {
  std::cout << "hello" << std::endl;
}
void world() {
  std::thread t1(hello);
  t1.join();
  std::cout << "world" << std::endl;
}

int main() {
  std::thread t2(world);

  t2.join();
}
