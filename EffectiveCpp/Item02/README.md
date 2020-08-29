# Prefer consts, enums, and inlines to #defines.

## static const defination and declaration inside a class

NumTurns is a declaration in following code. It works as expected in print() function without defination, but compiler complains undefined reference if you access the address of such variable

```C++
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
```

To avoid the compiler error, defination in implementation file is required. It is no need to initialize again in definiation since we initialized it with a value 5 in declaration. For example:

```C++
const int GamePlayer::NumTurns;
```
