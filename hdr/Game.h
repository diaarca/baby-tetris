#pragma once

#include "State.h"

class Game
{
  private:
    State state_;
    int score_;

  public:
    Game(Field& field);
    Game(State& state)
        : state_(std::move(state)), score_(0) {};

    State& getState() { return state_; };
    int getScore() { return score_; };

    void setState(State s) { state_ = std::move(s); };
    void setScore(int sc) { score_ = sc; };

    void playRandom();
};
