> Note that std::declval can only be used in [unevaluated contexts](https://en.cppreference.com/w/cpp/language/expressions#Potentially-evaluated_expressions) and is not required to be defined; it is an error to evaluate an expression that contains this function. Formally, the program is ill-formed if this function is odr-used.

- 为什么decltype返回的是万能引用`T&&`:
  - 引用是非完备类型，只需要声明不需要定义。非求值上下文中能够放宽类型限制。
  - 万能引用可以保留rvalue/lvalue属性。
