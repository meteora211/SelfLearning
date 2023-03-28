### custom class in `views::iota`

Following member function are REQUIRED by testing. Which denotes an Iterator type.
```
  using difference_type = std::ptrdiff_t;
  bool operator==(const TestIOTA& rhs) const;
  TestIOTA& operator++();
  TestIOTA operator++(int);
  TestIOTA() = default;
  float operator*();
```

### iterator in calender
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

The code `dates_between(2022, 2023) | by_month()` denotes a custom `viewable_range`:
```
  dates_between(2022, 2023) | by_month()

  dates_between(2022, 2023) | group_by(month_lambda) // month_lambda: [](Date& a, Date& b){ return a.month() == b.month();}

  group_by(month_lambda)(dates_range) // dates_range: dates_between(2022, 2023)

  GroupWrapper()(month_lamda)(dates_range)

  GroupWrapper()(dates_range, month_lamda)

  group_by_view{dates_range, month_lambda}

```

Expand the ranged for-loop in `print_range`:

```
// loop in print_range is:
// for (auto&& v : range) {loop-statement}
// where range is group_by_view{dates_range, month_lambda}
{
  auto && gbv = group_by_view{dates_range, month_lambda};
  auto __begin = ranges::begin(gbv);
  auto __end = ranges::end(gbv);
  for ( ; __begin != __end; ++__begin) {
    v = *__begin;
    loop-statement
  }
}
```

The core implementation is `group_iterator::operator++`, `group_iterator::operator*` and `group_by_view::begin`
