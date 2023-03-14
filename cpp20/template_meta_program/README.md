# IR template
Summarize:
- `Graph` and `Node` are defined to represent IR.
- `PathFinder` defines templates function/variable/alias to generate shortest Path with GIVEN(by Graph template arguments) IR.
  - `GetMinPath` function defined to find the shortest path.
  - alias `type` is the `TypeList` with shortest Path. (`Fold_t<GetMinPath>`)
- `CrossProduct` generates all possible Node pairs.
- `ReachableNodePairs` filters valid Node pairs.
- `SaveAllPath` maps `ReachableNodePairs` with `PathStorage`. The dumped type info is contained in [SaveAllPath_Dump](SaveAllPath_Dump)
- `PathStorage` stores Path as static member variable.(.rodata)

The most confused part is how to get Path at runtime:

`getPath` accept node id and match it from the INSTANCE of `SaveAllPath`.


```cpp
  template <typename NodeType>
  static constexpr PathRef<NodeType> getPath(NodeType from, NodeType to) {
    PathRef<NodeType> result{};
    /* dump<SaveAllPath>{}; */
    matchPath(from, to, result, SaveAllPath{});
    return result;
  }
```


`matchPath` function is overloaded with `void` return and `bool` return. `void` version
is called in `getPath` and iterates `bool` version.
The initialization of `SaveAllPath` is to create a dummy object, and use it's type info.
The shortest path is fetched to `result` only when the type info matches input `from` and `to`.
```
  template<typename NodeType, typename... PathPairs>
  static constexpr void matchPath(NodeType from,
                             NodeType to,
                             PathRef<NodeType>& result,
                             TypeList<PathPairs...>) {
    (matchPath(from, to, result, PathPairs{})||...);
  }

  template<typename NodeType, Vertex From, Vertex Target, typename PathStorage_>
  static constexpr bool matchPath(NodeType from,
                             NodeType to,
                             PathRef<NodeType>& result,
                             std::pair<std::pair<From, Target>, PathStorage_>) {
    if (From::id == from && Target::id == to) {
      result = PathStorage_::path;
      return true;
    }
    return false;
  }
```

The above logic applies to both runtime and compile time with constexpr function definition.
- in compile time, compiler iterates the given Node id `from` and `to` in `SaveAllPath`.
- in runtime, it is also able to iterates the input Node id `from` and `to` in the static data  `SaveAllPath{}` ->  `pair<pair<A,B>, PathStorage<A,B>` -> `PathStorage<A,B>::path`.

```
readelf -x .rodata test

  0x00002000 01000200 00000000 01000000 66726f6d ............from
  0x00002010 20002074 6f200020 70617468 2073697a  . to . path siz
  0x00002020 653a2000 2d3e0000 00000000 00000100 e: .->..........
  0x00002030 00000000 01000000 00000041 42414341 ...........ABACA
  0x00002040 42444141 45424243 42444241 42414543 BDAAEBBCBDBABAEC
  0x00002050 4344                                CD
```
