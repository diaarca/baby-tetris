#pragma once

#include "Game.h"
#include <vector>

class HeuristicPlayer
{
  public:
    HeuristicPlayer() = default;
    int playGame(Game& game, const std::vector<double>& weights, const std::unordered_map<State, std::unique_ptr<Tromino>>& advPolicy);
};
