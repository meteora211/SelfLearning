#include "GamePlayer.h"

#ifndef ENUM_HACK
// Defination:
// Note: no static is used in defination
const int GamePlayer::NumTurns;
#endif

int main() {
  GamePlayer g;
  g.print();
  // Following returns an compile error without defination
  // undefined reference to `GamePlayer::NumTurns'`
  g.getAddress();
}
