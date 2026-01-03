#include "Game.h"
#include "State.h"
#include <algorithm>

Game::Game(const std::array<int, 3>& config, Field& field)
    : state_(field, std::make_unique<IPiece>()), score_(0), config_(config)
{
    if ((rand() / (double)RAND_MAX) < PROBA_I_PIECE)
        state_ = State(field, std::make_unique<IPiece>());
    else
        state_ = State(field, std::make_unique<LPiece>());
}

void Game::playRandom()
{
    // Game loop
    int lines, i, gain;
    std::vector<Action> actions;
    lines = i = 0;
    std::cout << "Initial State:\n" << state_ << std::endl;
    std::cout << "Starting game...\n";

    while ((actions = state_.getAvailableActions()).size() > 0)
    {
        if (!actions.empty())
        {
            // random action for now
            Action a = actions[rand() % actions.size()];
            std::cout << "Action number " << i << std::endl;
            state_ = state_.applyAction(a);
            std::cout << state_;
            gain = state_.evaluate(config_);

            auto it = std::find(std::begin(config_), std::end(config_), gain);
            lines = std::distance(std::begin(config_), it) + 1;
            score_ += gain;
            if (lines > 0)
            {
                std::cout << "Action completed " << lines << " lines\n";
                std::cout << "New board:\n" << state_.getField();
            }
            state_ = state_.completeLines();
            std::cout << "Current score: " << score_ << "\n\n";
        }
        i++;
    }
    std::cout << "Game Over! Global score: " << score_ << "\n";
}
