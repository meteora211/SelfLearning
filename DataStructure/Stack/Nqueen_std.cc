#include <iostream>
#include <stack>
#include <vector>
#include <deque>
#include <algorithm>
using std::stack;
using std::vector;
using std::deque;

class Queen {
public:
  Queen(int xp, int yp) : x(xp), y(yp) {}
  int x;
  int y;
  bool operator==(const Queen& rhs) {
    return (x == rhs.x) || (y == rhs.y) || ((x + y) == (rhs.x + rhs.y)) || ((x - y) == (rhs.x - rhs.y));
  }
  bool operator!=(const Queen& rhs) {
    return !(*this==rhs);
  }
};

void placeQueen(int n, vector<Queen>& solution) {
  Queen q(0,0);
  while(solution.size() < n) {
    if (q.y >= n) {
      q = solution.back();
      solution.pop_back();
      ++(q.y);
    } else {
      while(q.y < n && (std::find(solution.begin(), solution.end(), q) != solution.end())) {
        q.y++;
      }
      if (q.y < n) {
        solution.push_back(q);
        q.y = 0;
        q.x++;
      }
    }

  }

}

int main() {
  int N = 8; // Nqueen problem
  vector<Queen> solution;
  placeQueen(N, solution);
  for (auto iter=solution.begin(); iter!=solution.end(); ++iter) {
    std::cout << "x: " << iter->x << " " << "y: " << iter->y << std::endl;
  }
}
