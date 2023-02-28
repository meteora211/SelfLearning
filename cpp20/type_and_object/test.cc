#include <iostream>

template <typename T>
class TemplateClass {
public:
  TemplateClass() = default;
  ~TemplateClass() = default;
  template<typename NewType>
  void template_function(NewType&& in);
};

template<typename T>
template<typename NewType>
void TemplateClass<T>::template_function(NewType&& in) {
  std::cout << "template func: " << in << std::endl;
}

int main() {
  auto t = TemplateClass<double>();
  t.template_function<int>(1);
}
