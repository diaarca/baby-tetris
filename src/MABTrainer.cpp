#include "MABTrainer.h"
#include <cmath>
#include <limits>
#include <iostream>
#include <sstream>
#include <iomanip>

MABTrainer::MABTrainer(Game& game, const std::unordered_map<State, std::unique_ptr<Tromino>>& advPolicy)
    : game_(game), advPolicy_(advPolicy)
{
    // Define the arms (heuristic weights)
    arms_ = {
        {-0.51, 0.76, -0.36, -0.18}, // Balanced (from a known paper)
        {-0.8, 0.5, -0.2, -0.1},     // Height-averse
        {-0.2, 0.8, -0.8, -0.3},     // Line-clearing-focused
        {-0.4, 0.6, -0.1, -0.8},     // Bumpiness-averse
        {-1.0, 1.0, -1.0, -1.0},     // Aggressive penalties
        {-0.1, 0.5, -0.5, -0.5}      // Cautious
    };

    plays_.assign(arms_.size(), 0);
    rewards_.assign(arms_.size(), 0.0);
    total_plays_ = 0;
}

void MABTrainer::train(int training_games)
{
    HeuristicPlayer player;

    for (int i = 0; i < training_games; ++i)
    {
        int arm_to_play = -1;

        // First, play each arm once
        for (size_t j = 0; j < arms_.size(); ++j)
        {
            if (plays_[j] == 0)
            {
                arm_to_play = j;
                break;
            }
        }

        // If all arms played, use UCB1
        if (arm_to_play == -1)
        {
            double max_ucb = -1.0;
            for (size_t j = 0; j < arms_.size(); ++j)
            {
                double mean_reward = rewards_[j] / plays_[j];
                double ucb_value = mean_reward + sqrt(2 * log(total_plays_) / plays_[j]);
                if (ucb_value > max_ucb)
                {
                    max_ucb = ucb_value;
                    arm_to_play = j;
                }
            }
        }

        // Play the game with the selected arm
        int score = player.playGame(game_, arms_[arm_to_play], advPolicy_);

        // Update stats
        plays_[arm_to_play]++;
        rewards_[arm_to_play] += score;
        total_plays_++;
        
        if ((i + 1) % 100 == 0)
        {
            std::cout << "Training game " << i + 1 << "/" << training_games << " completed." << std::endl;
        }
    }
}

std::vector<double> MABTrainer::getBestWeights() const
{
    double max_mean_reward = -std::numeric_limits<double>::infinity();
    int best_arm_index = -1;

    for (size_t i = 0; i < arms_.size(); ++i)
    {
        if (plays_[i] > 0)
        {
            double mean_reward = rewards_[i] / plays_[i];
            if (mean_reward > max_mean_reward)
            {
                max_mean_reward = mean_reward;
                best_arm_index = i;
            }
        }
    }
    return best_arm_index != -1 ? arms_[best_arm_index] : std::vector<double>();
}

std::string MABTrainer::getBestWeightsString() const
{
    auto weights = getBestWeights();
    if(weights.empty())
    {
        return "No weights found.";
    }

    std::ostringstream oss;
    oss << "[";
    for(size_t i = 0; i < weights.size(); ++i)
    {
        oss << std::fixed << std::setprecision(2) << weights[i] << (i < weights.size() - 1 ? ", " : "");
    }
    oss << "]";
    return oss.str();
}
