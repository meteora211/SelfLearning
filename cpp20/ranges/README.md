### custom class in `views::iota`

Following member function are REQUIRED by testing.
```
  using difference_type = std::ptrdiff_t;
  bool operator==(const TestIOTA& rhs) const;
  TestIOTA& operator++();
  TestIOTA operator++(int);
  TestIOTA() = default;
```

### iterator
Given ranged for loop:
```
for ( range-declaration : range-expression ) loop-statement

```
it can expand to following since c++17
```cpp
{
  auto && __range = range-expression ;
  auto __begin = begin-expr ;
  auto __end = end-expr ;
  for ( ; __begin != __end; ++__begin) {
    range-declaration = *__begin;
    loop-statement
  }
}
```

```
  dates_between(2022, 2023) | by_month()

  by_month()(dates_between(2022, 2023))

  group_by(lambda)(dates_between)

```


