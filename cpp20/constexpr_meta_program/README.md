### constinit, consteval, constexpr, const

#### constexpr vs const
- const can be applied to all kinds of objects to indicate their immutability
- const integral type with constant initialization can be used in constant expression
- constexpr object, by definition, can be used in constant expressions
- constexpr can be applied to functions to show that they can be called to produce constant expressions (they can also be called at runtime)
- const can be applied to member functions to indicate that a function doesn’t change data members (unless mutable),

#### constexpr vs consteval
- consteval can only be applied to functions
- constexpr can be applied to functions and also variables
- consteval forces compile time function evaluation; the constexpr function can be executed at compile time but also at runtime (as a regular function).

#### constexpr vs constinit
- constexpr variables are constant and usable in constant expressions
- constinit variables are not constant and cannot be used in constant expressions
- constexpr can be applied on local automatic variables; this is not possible with constinit, which can only work on static or thread_local objects
- you can have a constexpr function, but it’s not possible to have a constinit function declaration

reference: https://www.cppstories.com/2022/const-options-cpp20/


### fold expression
useful link:
- https://en.cppreference.com/w/cpp/language/fold
- https://www.fluentcpp.com/2021/03/12/cpp-fold-expressions
