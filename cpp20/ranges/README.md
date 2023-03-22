### custom class in `views::iota`

Following member function are REQUIRED by testing.
```
  using difference_type = std::ptrdiff_t;
  bool operator==(const TestIOTA& rhs) const;
  TestIOTA& operator++();
  TestIOTA operator++(int);
  TestIOTA() = default;
```
