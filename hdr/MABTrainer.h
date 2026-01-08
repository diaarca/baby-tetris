#pragma once

#include "Game.h"
#include "HeuristicPlayer.h"
#include <vector>
#include <string>

class MABTrainer
{
  private:
    std::vector<std::vector<double>> arms_;
    std::vector<int> plays_;
    std::vector<double> rewards_;
    int total_plays_;
    Game& game_;
    const std::unordered_map<State, std::unique_ptr<Tromino>>& advPolicy_;


  public:
    MABTrainer(Game& game, const std::unordered_map<State, std::unique_ptr<Tromino>>& advPolicy);
    void train(int training_games);
    std::vector<double> getBestWeights() const;
    std::string getBestWeightsString() const;
};
