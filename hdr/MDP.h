#pragma once

#include "Game.h"
#include <algorithm>
#include <cstddef>
#include <limits>
#include <float.h>
#include <memory>
#include <numeric>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define MAX_ACTION 10000
#define DEBUG 0

class MDP
{
  private:
    int width_;
    int height_;
    State s0_;

  public:
    MDP(int width, int height, State s0)
        : width_(width), height_(height), s0_(std::move(s0)) {};
    ~MDP() = default;

    std::unordered_map<State, Action>
    actionValueIteration(double lambda, double line_weight, double height_weight, double score_weight, double gap_reduction, double epsilon, int maxIteration);

    std::unordered_map<State, std::unique_ptr<Tromino>>
    trominoValueIterationMinMax(double epsilon,
                                int maxIteration,
                                double lambda);

    std::unordered_map<State, std::unique_ptr<Tromino>>
    trominoValueIterationMinAvg(double epsilon,
                                int maxIteration,
                                double lambda);

    std::unordered_map<State, std::unique_ptr<Tromino>>
    trominoValueIterationGapAvg(double epsilon,
                             int maxIteration,
                             double lambda);

    std::unordered_map<State, double> generateReachableStates(State s0);

    int playPolicy(
        Game& game,
        const std::unordered_map<State, Action>& policy,
        const std::unordered_map<State, std::unique_ptr<Tromino>>& advPolicy);

    void prettyPrint(State& curr, State placed, State after);



  private:
    int getMaxHeight(const Field& field) const;
};
