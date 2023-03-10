#pragma once
#include "template_traits.h"
#include <string>
#include <type_traits>
#include <concepts>

template<char ID>
struct Node {
  static constexpr char id = Id;
};

template<typename... Chains>
class Graph {};

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
  using IsFrom = std::is_same<Edge::From, Node>;
  template<typename Edge>
  using IsTo = std::is_same<Edge::To, Node>;
  template<typename Edge>
  using GetFrom = typename Edge::From;
  template<typename Edge>
  using GetTo = typename Edge::To;
};


