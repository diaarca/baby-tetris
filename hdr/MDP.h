#pragma once

#include "Game.h"
#include <float.h>
#include <memory>
#include <numeric>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define MAX_ACTION 1000

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

    std::unordered_map<State, Action>
    actionValueIterationExpl(double eps, int maxIteration, double lambda);
    std::vector<Action>
    actionValueIteration(double eps, int maxIteration, double lambda);

    std::vector<std::unique_ptr<Tromino>>
    trominoValueIteration(double epsilon, int maxIteration, double lambda);

    std::unordered_map<State, double> generateReachableStates(State s0);
    std::vector<State> generateAllStates();

    size_t stateIndex(const State& s);

    void playPolicy(Game& game,
                    const std::unordered_map<State, Action>& policy,
                    const std::vector<std::unique_ptr<Tromino>>& advPolicy);
};
