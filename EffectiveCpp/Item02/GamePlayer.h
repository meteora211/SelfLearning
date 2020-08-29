#include <iostream>

#define ASPECT_RATIO 1.653
#define ENUM_HACK

#ifndef ENUM_HACK
class GamePlayer {
private:
  // Declaration
  static const int NumTurns = 5;
  int scores[NumTurns];
public:
  void print() {
    std::cout << "current turn: " << NumTurns << std::endl;
  }
  void getAddress() {
    std::cout << "address of current turn: " << &NumTurns << std::endl;
  }
};
#else
class GamePlayer {
private:
  enum {NumTurns = 5};
  int scores[NumTurns];
public:
  void print() {
    std::cout << "current turn: " << NumTurns << std::endl;
  }
  void getAddress() {
    std::cout << "address of current turn:  Cannot retrieve address of enum" << std::endl;
  }
};

#endif

