## 模板类继承

* 模板类继承编译错误
头文件Stack.h中定义Stack.h如下: 
```cpp
template <typename T> 
class Stack : public Vector<T> {
public:
    void push(const T & e);
    T pop();
    T& top();
};
```
main函数里面调用函数如下
```cpp
Stack<int> s;
s.push(1);
```
编译的时候报错：
error: there are no arguments to ‘size’ that depend on a template parameter, so a declaration of ‘size’ must be available [-fpermissive]

报错的原因在于template类的继承中，编译器在寻找命名时与普通类有一些不同。考虑Stack<int>与Stack<double>是两种不同的类，其内部的成员函数也会不同，因此，在没有指定类型的情况下，编译器没有办法判断template类中的名字是否来自于父类。

解决方法一般有两种<sup>[1]</sup>：
1. 使用this指针表明size()函数是Stack类中的成员函数，这样编译器就会向父类去寻找size()
2. 在Stack类中作出如下声明, 表明使用父类中的size和insert函数。
```cpp
    using Vector<T>::size;
    using Vector<T>::insert;
``` 

再次编译出现了以下的错误:
```shell
Meteora# g++ -std=c++11 Stack_main.cc -o main
/tmp/ccTkeXOk.o: In function `Stack<int>::push(int const&)':
Stack_main.cc:(.text._ZN5StackIiE4pushERKi[_ZN5StackIiE4pushERKi]+0x18): undefined reference to `Vector<int>::size() const'
Stack_main.cc:(.text._ZN5StackIiE4pushERKi[_ZN5StackIiE4pushERKi]+0x2c): undefined reference to `Vector<int>::insert(int, int const&)'
collect2: error: ld returned 1 exit status
```
在Vector类模板的头文件末尾include Vector类的实现即可<sup>[2]</sup>。




REF:

[1] https://stackoverflow.com/questions/5286922/g-template-parameter-error
[2] https://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
