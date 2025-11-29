#pragma once

#include "State.h"

class Game
{
  private:
    State state_;
    int score_;
    std::array<int, 3> config_;

  public:
    Game(const std::array<int, 3>& config, Field& field);

    State& getState() { return state_; };
    int getScore() { return score_; };
    std::array<int, 3> getConfig() { return config_; };

    void setState(State s) { state_ = std::move(s); };
    void setScore(int sc) { score_ = sc; };

    void playRandom();
};
