#pragma once

#include "Action.h"
#include "Game.h"
#include "State.h"
#include <cmath>
#include <vector>

class MDP
{
  private:
    double pIPiece_;
    int width_;
    int height_;
    State s0_;
    std::array<int, 3> config_;

  public:
    MDP(double p, int width, int height, State s0, std::array<int, 3>& config)
        : pIPiece_(p), width_(width), height_(height), s0_(std::move(s0)),
          config_(std::move(config)) {};
    ~MDP() = default;

    int getP() { return pIPiece_; };
    std::vector<Action>
    valueIteration(double eps, int maxIteration, double lambda);

    // Generate all possible states for the current field dimensions.
    // Each cell can be occupied or empty, and for each grid the next
    // tromino can be one of the available piece types (IPiece/LPiece).
    // Returns a vector containing every possible `State`.
    std::vector<State> generateAllStates() const;

    // Compute the index of a given `State` in the vector returned by
    // `generateAllStates()`. If the state cannot be mapped (e.g. grid
    // size too large for enumeration or unknown piece), returns
    // `static_cast<size_t>(-1)`.
    size_t stateIndex(const State& s) const;

    void playPolicy(Game& game, std::vector<Action> policy);
};
