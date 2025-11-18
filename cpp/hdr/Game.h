#include "Field.h"
#include "State.h"
#include "Tromino.h"
#include <cmath>
#include <iostream>

class Game
{
  private:
    State state;
    int score;
    std::array<int, 3> config;

  public:
    Game(const std::array<int, 3>& config, Field& field);
    void play();
    int completeLine();
};
