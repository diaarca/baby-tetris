#pragma once

#include "Game.h"
#include <sstream>

#define MAX_ACTION 10000

class MDP
{
  private:
    int width_;
    int height_;
    State s0_;
    std::array<int, 3> config_;

  public:
    MDP(int width, int height, State s0, std::array<int, 3>& config)
        : width_(width), height_(height), s0_(std::move(s0)),
          config_(std::move(config)) {};
    ~MDP() = default;

    std::vector<Action>
    valueIteration(double eps, int maxIteration, double lambda);

    std::vector<State> generateAllStates();

    size_t stateIndex(State& s);

    void playPolicy(Game& game, std::vector<Action> policy);
};
