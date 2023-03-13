#include "ir_template.h"
#include <string>
#include <type_traits>
#include <concepts>

int main() {
  using A = Node<'A'>;
  using B = Node<'B'>;
  using C = Node<'C'>;
  using D = Node<'D'>;
  using E = Node<'E'>;

  using g = Graph<
      link(node(A) -> node(B) -> node(C) -> node(D)),
      link(node(A) -> node(C)),   // test shortest path: A -> C -> D
      link(node(B) -> node(A)),   // test cycle
      link(node(A) -> node(E)) >; // test D -> E unreachable

  /* static_assert(g::GetPath('A', 'D').sz == 3); // compile-time test */
  /* static_assert(g::getPath('A', 'A').sz == 1); */
  /* static_assert(g::getPath('B', 'D').sz == 3); */
  /* static_assert(g::getPath('B', 'E').sz == 3); */
  /* static_assert(g::getPath('D', 'E').sz == 0); */
}
