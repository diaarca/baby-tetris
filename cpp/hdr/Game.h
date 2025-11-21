#pragma once

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
    void playRandom();
    State& getState() { return state; };
    int getScore() { return score; };
    std::array<int, 3> getConfig() { return config; };
    void setState(State s) { state = std::move(s); };
    void setScore(int sc) { score = sc; };
};
