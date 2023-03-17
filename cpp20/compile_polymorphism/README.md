### Empty Base Optimization
[EBO](https://en.cppreference.com/w/cpp/language/ebo)
- by optimization in derived class
- by `[[no_unique_class]]`

This idiom is used in STL(vector, unique_ptr, etc.), take MSVC as example:

MSVC defines `_Compressed_pair<_Dx, pointer>` as the only member variable in [unique_ptr](https://github.com/microsoft/STL/blob/daa994bfc41c36196c536f2b68388f859d6bd656/stl/inc/memory#L3318)

```cpp
_EXPORT_STD template <class _Ty, class _Dx /* = default_delete<_Ty> */>
class unique_ptr { // non-copyable pointer to an object
private:
    _Compressed_pair<_Dx, pointer> _Mypair;
};
```

Thus `sizeof(unique_ptr) == sizeof(__Compressed_pair<_Dx, pointer>)`. And [_Compressed_pair<_Dx, pointer>](https://github.com/microsoft/STL/blob/daa994bfc41c36196c536f2b68388f859d6bd656/stl/inc/xmemory#L1495) is defined as following,
the function `_Get_first` is also listed to illustrate how deleter is called later.

```
template <class _Ty1, class _Ty2, bool = is_empty_v<_Ty1> && !is_final_v<_Ty1>>
class _Compressed_pair final : private _Ty1 { // store a pair of values, deriving from empty first
public:
    _Ty2 _Myval2;

    using _Mybase = _Ty1; // for visualization

    constexpr _Ty1& _Get_first() noexcept {
        return *this;
    }

    constexpr const _Ty1& _Get_first() const noexcept {
        return *this;
    }
};

template <class _Ty1, class _Ty2>
class _Compressed_pair<_Ty1, _Ty2, false> final { // store a pair of values, not deriving from first
public:
    _Ty1 _Myval1;
    _Ty2 _Myval2;

    constexpr _Ty1& _Get_first() noexcept {
        return _Myval1;
    }

    constexpr const _Ty1& _Get_first() const noexcept {
        return _Myval1;
    }
};
```

`_Compressed_pair` derived from `_Ty1`(which is the deleter type `_Dx` in `unique_ptr`) when `_Ty1` is not final and is an empty class.

EBO will work when above condition is meet and thus `sizeof(unique_ptr) == sizeof(__Compressed_pair<_Dx, pointer>) == sizeof(_Dx)`.

The another question is HOW unique_ptr releases its resource with deleter? Check the [dtor](https://github.com/microsoft/STL/blob/daa994bfc41c36196c536f2b68388f859d6bd656/stl/inc/memory#L3270) of unique_ptr:

```
    _CONSTEXPR23 ~unique_ptr() noexcept {
        if (_Mypair._Myval2) {
            _Mypair._Get_first()(_Mypair._Myval2);
        }
    }
```

We see `_Get_first` is called to get the actual deleter function:
- For EBO case, a `this` pointer is returned and casted to `_Ty1` due to inheritance.
- For non-EBO case, deleter is defined as member variable `_Myval1`. So just return it.

### Reference:
- https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Empty_Base_Optimization
