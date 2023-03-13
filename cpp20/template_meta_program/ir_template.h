#pragma once
#include "template_traits.h"
#include <string>
#include <type_traits>
#include <concepts>


#define node(node) auto(*) (node)
#define link(link) link -> void

template<char ID>
struct Node {
  static constexpr char id = ID;
};

template<typename Node>
concept Vertex = requires {
  Node::id;
};

template<Vertex F, Vertex T>
struct Edge {
  using From = F;
  using To = T;
};

template<typename Node = void>
requires (Vertex<Node> || std::is_void_v<Node>)
struct EdgeTrait {
  template<typename Edge>
  using IsFrom = std::is_same<typename Edge::From, Node>;
  template<typename Edge>
  using IsTo = std::is_same<typename Edge::To, Node>;
  template<typename Edge>
  using GetFrom = typename Edge::From;
  template<typename Edge>
  using GetTo = typename Edge::To;
};

template<typename Link, TL Out = TypeList<>>
struct Chain;
template<Vertex F, TL Out>
struct Chain<auto(*)(F) -> void, Out> {
  using From = F;
  using type = Out;
};
template<Vertex F, typename T, TL Out>
struct Chain<auto(*)(F) -> T, Out> {
  using From = F;
  using To = typename Chain<T, Out>::From;
  using type = typename Chain<T, typename Out::template push<Edge<From, To>>>::type;
};

template<typename Link, TL Out = TypeList<>>
using Chain_t = Chain<Link, Out>::type;

template <typename... Chain>
struct Graph {
  using Edges = Unique<Concat_t<Chain_t<Chain>...>>;

  template<Vertex CurrNode, Vertex Target, TL Path>
  struct PathFinder;
  // end condition
  // if from == target: return path
  // XXX: Following default template parameter cause compiler error:
  // default template arguments may not be used in partial specializations
  // template<Vertex Target, TL Path = TypeList<>>
  // Which is due to __Default arguments cannot appear in the argument list__
  // See https://en.cppreference.com/w/cpp/language/partial_specialization
  template<Vertex Target, TL Path>
  struct PathFinder<Target, Target, Path>: Path::template push<Target> {};
  // loop detected
  // if from in path: return []
  template<Vertex From, Vertex Target, TL Path>
  requires (Elem_v<Path, From>)
  struct PathFinder<From, Target, Path> : TypeList<> {};
  // DFS
  template<Vertex From, Vertex Target, TL Path = TypeList<>>
  struct PathFinder {
    using EdgesFrom = Filter_t<EdgeTrait<From>::template IsFrom, Edges>;
    using NextNodes = Map_t<EdgeTrait<>::GetTo, EdgesFrom>;

    template<Vertex AdjacentNode>
    using GetPath = PathFinder<AdjacentNode, Target,typename Path::template push<From>>;
    using AllPathFromCurNode = Map_t<GetPath, NextNodes>;

    template<TL AccMinPath, TL Path_>
    using GetMinPath = std::conditional_t<AccMinPath::size == 0 ||
                                          (AccMinPath::size > Path_::size && Path_::size > 0),
                                          Path_, AccMinPath>;

    using type = Fold_t<GetMinPath, AllPathFromCurNode, TypeList<>>;
  };

};
