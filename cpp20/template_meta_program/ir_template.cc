#include "ir_template.h"
#include <string>
#include <type_traits>
#include <concepts>

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

static_assert(g::getPath('A', 'D').sz == 3); // compile-time test
static_assert(g::getPath('A', 'A').sz == 1);
static_assert(g::getPath('B', 'D').sz == 3);
static_assert(g::getPath('B', 'E').sz == 3);
static_assert(g::getPath('D', 'E').sz == 0);

int main(int argc, char** argv) {
  char from = 'A';
  char to = 'D';
  if (argc > 2) {
      from = argv[1][0]; // A
      to = argv[2][0]; // D
  }
  auto path = g::getPath(from, to); // runtime test
  std::cout << "from " << from << " to " << to
      << " path size: " << path.sz << std::endl;
  for (size_t i = 0; i < path.sz; ++i) {
      std::cout << path.path[i];
      if (i != path.sz - 1) {
          std::cout << "->";
      }
  }
  std::cout << std::endl;

  return 0;
}

